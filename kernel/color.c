#include <linux/linkage.h>
#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/cred.h>
#include <linux/types.h>
#include <linux/sched.h>
#include <asm/uaccess.h>
#define debug 1
void print_task(struct task_struct* tsk)
{
      printk("-------Task------\n");
      printk("pid: %d\n", tsk->pid);
      printk("tgid: %d\n",tsk->tgid);
      printk("-------End------\n");
}
void print_pids(int *arr, int n, char* message)
{
   int i;
   if(message)
         printk("%s:", message);
   for(i=0; i<n; i++)   {
      printk("%d ", arr[i]);
   }
   printk("\n");
}

void print_colors(u_int16_t *arr, int n, char* message)
{
   int i;
   if(message)
         printk("%s:", message);
   for(i=0; i<n; i++)   {
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
      struct task_struct *sibling_entry;
      struct task_struct *child_entry;
      struct list_head* sibling;
      struct list_head* child;

      //if not root, return error
      if(current->real_cred->uid)
            return -EACCES;
      if(copy_from_user(kpids, pids, sizeof(pid_t)*nr_pids))
            return -EFAULT;
      if(copy_from_user(kcolors, colors, sizeof(u_int16_t)*nr_pids))
            return -EFAULT;

      if(debug){
            printk("-----SET_COLORS------\n");
            print_pids(kpids, nr_pids, "Output kpids:");
            print_colors(kcolors, nr_pids, "Output kcolor:");
      }
      for(i=0;i<nr_pids;i++){
            struct task_struct *tmp_task;
            struct task_struct *task;
            struct task_struct * thread_task;
            pid_t  tgid;

            //need to lock pid???
            tmp_task = find_task_by_vpid(kpids[i]);
            if(!task){
                   kretval[i]=-EINVAL;
                   printk("Found no task by %d\n", kpids[i]);
                   continue;
            }
            tgid =tmp_task->tgid;
            if(debug)
                  print_task(tmp_task);

            continue;
            write_lock_irq(&tasklist_lock);
            do_each_thread(task, thread_task){
                  if(thread_task->tgid = tgid){
                  //unsigned long flags=0;
                  //write_lock_irqsave(&lock, flags);
                        thread_task->color = kcolors[i];
                        kretval[i] = 0;
                  //write_unlock_irqrestore(&lock, flags);
                  }
            }while_each_thread(task, thread_task);
            write_unlock(&tasklist_lock);
/*-------------------------------------------------------------------------------------------------------------------------------
Maybe I don't need this part

            list_for_each(child,&task->children){
                  if((child_entry=list_entry(child, struct task_struct, children))->tgid==tgid){
                        unsigned long flags=0;
                        child_enry->tasklist_lock = RW_LOCK_UNLOCKED;
                        write_lock_irqsave(&child_entry->tasklist_lock, flags);
                        child_entry->color = colors[i];
                        write_unlock_irqsotre(&child_entry->tasklist_lock, flags);
                  }
            }

            list_for_each(sibling, &task->sibling){
                  if((sibling_entry=list_entry(sibling, struct task_struct, sibling))->tgid == tgid){
                        list_for_each(child,sibling_entry){
                              if((child_entry=list_entry(child, struct task_struct, children))->tgid==tgid){
                                    unsigned long flags=0;
                                    child_enry->tasklist_lock = RW_LOCK_UNLOCKED;
                                    write_lock_irqsave(&child_entry->tasklist_lock, flags);
                                    child_entry->color = colors[i];
                                    write_unlock_irqstore(&child_entry->tasklist_lock, flags);
                              }
                        }
                  }
            }
 ------------------------------------------------------------------------------------------------------------------------------------*/
      }
      if(copy_to_user(retval,kretval, sizeof(int)*nr_pids))
            return -EFAULT;
      print_pids(retval, nr_pids, "retval:");
}

SYSCALL_DEFINE4(get_colors, int, nr_pids, pid_t *, pids, u_int16_t *, colors, int *, retval)
{
      pid_t  kpids[nr_pids];
      u_int16_t kcolors[nr_pids];
      int   kretval[nr_pids];
      int   i;
      struct task_struct *task;
      struct task_struct *sibling_entry;
      struct task_struct *child_entry;
      struct list_head* sibling;
      struct list_head* child;
      pid_t  tgid;

      printk("-----GET_COLORS------\n");

      if(copy_from_user(kpids, pids, sizeof(pid_t)*nr_pids))
            return -EFAULT;

      if(debug){
            print_pids(kpids, nr_pids, "Output kpids:");
      }

      return 0;
      for(i=0; i<nr_pids; i++){
            task = find_task_by_vpid(kpids[i]);
            if(!task){
                   kretval[i]=-EINVAL;
                   continue;
            }
            unsigned long flags;
            read_lock_irqsave(&lock, flags);
            kcolors[i] = task->color;
            kretval[i] = 0;
            read_unlock_irqrestore(&lock, flags);
      }
      if(copy_to_user(retval,kretval, sizeof(int)*nr_pids))
            return -EFAULT;
}
