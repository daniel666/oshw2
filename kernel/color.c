#include <linux/linkage.h>
#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/cred.h>
#include <linux/types.h>
#include <linux/sched.h>
#include <asm/uaccess.h>
#include <linux/pid.h>
#define debug 1
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
void print_pids(pid_t *arr, int n, char* message)
{
   int i;
   if(message)
         printk("%s:", message);
   for(i=0; i<n; i++)   {
      printk("%hu ", arr[i]);
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
rwlock_t lock = RW_LOCK_UNLOCKED;

SYSCALL_DEFINE4(set_colors, int, nr_pids, pid_t *, pids, u_int16_t *, colors, int *, retval)
{
      pid_t  kpids[nr_pids];
      u_int16_t kcolors[nr_pids];
      int   kretval[nr_pids] ;
      int   i;

      printk("\n\n\n-----SET_COLORS------\n");
      if(!(pids&&colors&&retval)||nr_pids<=0){
            printk("Illegal\n");
            return -1;
      }
      //if not root, return error
      /*if(debug)
            printk("UID: %d\n", current->real_cred->uid)
      if(current->real_cred->uid)
            return -EACCES;*/
      if(current->cred->uid)
            return -EACCES;
      printk("UID: %d\n", current->cred->uid);
      if(copy_from_user(kpids, pids, sizeof(pid_t)*nr_pids))
            return -EFAULT;
      if(copy_from_user(kcolors, colors, sizeof(u_int16_t)*nr_pids))
            return -EFAULT;

      if(debug){
            //print_pids(kpids, nr_pids, "Output kpids:");
            print_int(kpids, nr_pids, "Output kpids:"); //nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnote
            print_u_int16(kcolors, nr_pids, "Output kcolor:");
      }
      for(i=0;i<nr_pids;i++){
            struct task_struct *tmp_task;
            struct task_struct *task;
            struct task_struct *thread_task;
            pid_t  tgid;

            //need to lock pid???
            //tmp_task = find_task_by_vpid(kpids[i]);
            //tmp_task = pid_task(find_vpid(kpids[i]), PIDTYPE_PID);
            //task = pid_task(find_get_pid(kpids[i]), PIDTYPE_PID);
            task = find_task_by_vpid(kpids[i]);
            if(!task){
                   kretval[i]=-EINVAL;
                   printk("Found no task by %d\n", kpids[i]);
                   continue;
            }
            tgid =task->tgid;
            if(debug)
                  print_task(task);

//            do_each_thread(task, thread_task){
//                  if(thread_task->tgid == tgid){
//                        thread_task->color = kcolors[i];
//                        kretval[i] = 0;
//                  }
//            }while_each_thread(task, thread_task);
           thread_task = task;
           do{
                  write_lock_irq(&tasklist_lock);
                  if(debug)
                        printk("Assigning color to pid %d with color %hu\n",thread_task->pid, kcolors[i]);
                  thread_task->color = kcolors[i];
                  kretval[i] = 0;
                  write_unlock_irq(&tasklist_lock);
           }while_each_thread(task, thread_task); //note this macro iterates all threads in the thread_group cos it.\
                                                   we don't need to test if tgid are equal, it's semantically in the macro

      }
      if(copy_to_user(retval,kretval, sizeof(int)*nr_pids))
            return -EFAULT;
      print_int(retval, nr_pids, "retval:");

      return 0;
}

SYSCALL_DEFINE4(get_colors, int, nr_pids, pid_t *, pids, u_int16_t *, colors, int *, retval)
{
      pid_t  kpids[nr_pids];
      u_int16_t kcolors[nr_pids];
      int   kretval[nr_pids];
      int   i;
      pid_t  tgid;

      printk("\n-----GET_COLORS------\n");
      if(!(pids&&colors&&retval)||nr_pids<=0)
            return -1;

      if(copy_from_user(kpids, pids, sizeof(pid_t)*nr_pids)) {
	    printk("[color.c] copy_from_user error\n");
            return -EFAULT;
	}

      if(debug){
            print_int(kpids, nr_pids, "Output kpids:");
      }

      for(i=0; i<nr_pids; i++){
            struct task_struct *task;

            task = find_task_by_vpid(kpids[i]);
            if(!task){
                   kretval[i]=-EINVAL;
                   continue;
            }
            if(debug)
                  print_task(task);

            //unsigned long flags;
            //read_lock_irqsave(&lock, flags);
            read_lock(&tasklist_lock);
            kcolors[i] = task->color;
            kretval[i] = 0;
            read_unlock(&tasklist_lock);
            //read_unlock_irqrestore(&lock, flags);
      }
      if(debug){
            print_u_int16(kcolors, nr_pids, "fetched kcolors:");
            print_int(kretval, nr_pids, "kretval:");
      }
      if(copy_to_user(retval,kretval, sizeof(int)*nr_pids))
            return -EFAULT;

      if(copy_to_user(colors,kcolors, sizeof(int)*nr_pids))
            return -EFAULT;

      return 0;
}
