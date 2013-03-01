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
      printk("pid: %d\n", tsk->pid);
      printk("tgid: %d\n",tsk->tgid);
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
      printk("%d ", arr[i]);
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
      bool  fail_flag=false;

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
            print_int(kpids, nr_pids, "Output kpids:");
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
            tmp_task = pid_task(find_get_pid(kpids[i]), PIDTYPE_PID);
            if(!tmp_task){
                   kretval[i]=-EINVAL;
                   printk("Found no task by %d\n", kpids[i]);
                   fail_flag=true;
                   continue;
            }
            tgid =tmp_task->tgid;
            if(debug)
                  print_task(tmp_task);

            write_lock_irq(&tasklist_lock);
            do_each_thread(task, thread_task){
                  if(thread_task->tgid == tgid){
                  //unsigned long flags=0;
                  //write_lock_irqsave(&lock, flags);
                        thread_task->color = kcolors[i];
                        kretval[i] = 0;
                  //write_unlock_irqrestore(&lock, flags);
                  }
            }while_each_thread(task, thread_task);
            write_unlock(&tasklist_lock);
      }
      if(copy_to_user(retval,kretval, sizeof(int)*nr_pids))
            return -EFAULT;
      print_int(retval, nr_pids, "retval:");

      if(fail_flag) return -1;
      return 0;
}

SYSCALL_DEFINE4(get_colors, int, nr_pids, pid_t *, pids, u_int16_t *, colors, int *, retval)
{
      pid_t  kpids[nr_pids];
      u_int16_t kcolors[nr_pids];
      int   kretval[nr_pids];
      int   i;
      pid_t  tgid;
      bool  fail_flag=false;

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
                   fail_flag=true;
                   continue;
            }
            if(debug)
                  print_task(task);

            //unsigned long flags;
            //read_lock_irqsave(&lock, flags);
            read_lock_irq(&tasklist_lock);
            kcolors[i] = task->color;
            kretval[i] = 0;
            read_unlock_irq(&tasklist_lock);
            //read_unlock_irqrestore(&lock, flags);
      }
      if(debug){
            print_u_int16(kcolors, nr_pids, "fetched kcolors:");
            print_int(kretval, nr_pids, "kretval:");
      }
      if(copy_to_user(retval,kretval, sizeof(int)*nr_pids))
            return -EFAULT;
	}

      if(fail_flag) return -1;
      return 0;
}
