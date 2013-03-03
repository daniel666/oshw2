#include <linux/linkage.h>
#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/cred.h>
#include <linux/types.h>
#include <linux/sched.h>
#include <asm/uaccess.h>
#include <linux/pid.h>
#include <linux/slab.h>
#define debug 1
/*
 *These are utitlity  functions
 */
void print_task(struct task_struct* tsk)
{
      printk("-------Task------\n");
      printk("pid: %hu\n", tsk->pid);
      printk("tgid: %hu\n",tsk->tgid);
      printk("name: %s\n",tsk->comm);
      printk("-------End------\n");
}

void print_int(int *arr, int n, char* message)
{
   int i;
   if(message)
         printk("%s:", message);
   for(i=0; i<n; i++)   {
      printk("%d ", arr[i]);
   }
   printk("\n");
}

void print_u_int16(u_int16_t *arr, int n, char* message)
{
   int i;
   if(message)
         printk("%s:", message);
   for(i=0; i<n; i++){
      printk("%hu ", arr[i]);
   }
   printk("\n");
}

SYSCALL_DEFINE4(set_colors, int, nr_pids, pid_t *, pids, u_int16_t *, colors, int *, retval)
{
        pid_t*  kpids;
        u_int16_t* kcolors;
        int*   kretval;
        int   i;

      if(debug){
            printk("\n\n\n-----SET_COLORS------\n");
      }
      if(nr_pids <=0)
            return -EINVAL;
      if(!(pids&&colors&&retval))
            return -EFAULT;
      if(current->cred->uid){
            return -EACCES;
      }


      kpids = kmalloc(nr_pids*sizeof(pid_t), GFP_KERNEL);
      kcolors = kmalloc(nr_pids*sizeof(u_int16_t), GFP_KERNEL);
      kretval = kmalloc(nr_pids*sizeof(int), GFP_KERNEL);
      if(!(kpids&&kcolors&&kretval)){
            printk("Error in kmllocating storage space\n");
            return -1;
      }

      if(copy_from_user(kpids, pids, sizeof(pid_t)* nr_pids)){
            kfree(kpids);
            kfree(kcolors);
            kfree(kretval);
            if(debug)
                  printk("Copy from user failed\n");
            return -EFAULT;
      }
      if(copy_from_user(kcolors, colors, sizeof(u_int16_t)*nr_pids)){
            kfree(kpids);
            kfree(kcolors);
            kfree(kretval);
            if(debug)
                  printk("Copy from user failed\n");
            return -EFAULT;
      }

      if(debug){
            print_int(kpids, nr_pids, "Output kpids:"); //nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnote
            print_u_int16(kcolors, nr_pids, "Output kcolor:");
      }

      for(i=0;i<nr_pids;i++){
            struct task_struct *task;
            struct task_struct *thread_task;

            rcu_read_lock();
            task = find_task_by_vpid(kpids[i]);
            if(!task){
                   kretval[i]=-EINVAL;
                   if(debug)
                        printk("Found no task by pid %d\n", kpids[i]);
                   rcu_read_unlock();
                   continue;
            }

            if(debug)
                  print_task(task);

           thread_task = task;
           do{
                  write_lock_irq(&tasklist_lock);
                  if(debug)
                        printk("Assigning color to pid %d with color %hu\n",thread_task->pid, kcolors[i]);
                  thread_task->color = kcolors[i];
                  kretval[i] = 0;
                  write_unlock_irq(&tasklist_lock);
           }while_each_thread(task, thread_task); //this macro iterates all threads in the thread_group cos it.\
                                                  great now we don't need to test if tgid are equal, it's semantically in the macro
          rcu_read_unlock();
      }
      if(copy_to_user(retval,kretval, sizeof(int)*nr_pids)){
            kfree(kpids);
            kfree(kcolors);
            kfree(kretval);
            if(debug)
                  printk("\n\n\n-----Copy To User failed------\n");
            return -EFAULT;
      }

      kfree(kpids);
      kfree(kcolors);
      kfree(kretval);

      if(debug)
         print_int(retval, nr_pids, "retval:");
      return 0;
}

SYSCALL_DEFINE4(get_colors, int, nr_pids, pid_t *, pids, u_int16_t *, colors, int *, retval)
{
      pid_t*  kpids;
      u_int16_t* kcolors;
      int*   kretval;
      int   i;

      if(debug)
            printk("\n-----GET_COLORS------\n");

      if(nr_pids <=0)
            return -EINVAL;
      if(!(pids&&colors&&retval))
            return -EFAULT;
      if(current->cred->uid){
            return -EACCES;
      }

      kpids = kmalloc(nr_pids*sizeof(pid_t), GFP_KERNEL );
      kcolors = kmalloc(nr_pids*sizeof(u_int16_t), GFP_KERNEL );
      kretval = kmalloc(nr_pids*sizeof(int), GFP_KERNEL );

      if(copy_from_user(kpids, pids, sizeof(pid_t)*nr_pids)){
            kfree(kpids);
            kfree(kcolors);
            kfree(kretval);
            if(debug)
                  printk("\n\n\n-----Copy From User failed------\n");
            return -EFAULT;
      }

      for(i=0; i<nr_pids; i++){
            struct task_struct *task;

            rcu_read_lock();
            task = find_task_by_vpid(kpids[i]);
            if(!task){
                   if(debug)
                        printk("Found no task by pid %d\n", kpids[i]);
                   kretval[i]=-EINVAL;
                   rcu_read_unlock();
                   continue;
            }
            if(debug)
                  print_task(task);

            read_lock(&tasklist_lock);
            kcolors[i] = task->color;
            kretval[i] = 0;
            read_unlock(&tasklist_lock);
            rcu_read_unlock();
      }

      if(debug){
            print_u_int16(kcolors, nr_pids, "fetched kcolors:");
            print_int(kretval, nr_pids, "kretval:");
      }
      if(copy_to_user(retval,kretval, sizeof(int)*nr_pids)){
            kfree(kpids);
            kfree(kcolors);
            kfree(kretval);
            if(debug)
                  printk("\n\n\n-----Copy To User failed------\n");
            return -EFAULT;
      }

      if(copy_to_user(colors,kcolors, sizeof(u_int16_t)*nr_pids)){
            kfree(kpids);
            kfree(kcolors);
            kfree(kretval);
            if(debug)
                  printk("\n\n\n-----Copy To User failed------\n");
            return -EFAULT;
      }
      kfree(kpids);
      kfree(kcolors);
      kfree(kretval);
      return 0;
}
