#include <linux/linkage.h>
#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/cred.h>
#include <linux/types.h>
#include <linux/sched.h>
#include <asm/uaccess.h>
#define debug 1
void print_int_2d(int *arr, int n, char* message)
{
   int i;
   if(message)
         printk("%s\n", message);
   for(i=0; i<n; i++)   {
      printk("%d ", arr[i]);
   }
}

rwlock_t lock = RW_LOCK_UNLOCKED;

SYSCALL_DEFINE4(set_colors, int, nr_pids, pid_t *, pids, u_int16_t *, colors, int *, retval)
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

      if(debug){
            printk("IN set_color\n");
            print_int_2d(pids, nr_pids, "Output pids in kernel");
            print_int_2d(colors, nr_pids, "Output color info");
      }
      //if not root, return error
      if(current->real_cred->uid)
            return -EACCES;
      if(copy_from_user(kpids, pids, sizeof(nr_pids)))
            return -EFAULT;
      if(copy_from_user(kcolors, colors, sizeof(nr_pids)))
            return -EFAULT;

      if(debug){
            print_int_2d(kpids, nr_pids, "Output kpids in kernel");
            print_int_2d(kcolors, nr_pids, "Output kcolor info");
      }
      printk("HERE\n");
      for(i=0;i<nr_pids;i++){
            //need to lock pid???
            task = find_task_by_vpid(kpids[i]);
            if(!task){
                   kretval[i]=-EINVAL;
                   continue;
            }
            tgid = task->tgid;
            struct task_struct * thread_task;
            while_each_thread(thread_task, task){
                  unsigned long flags=0;
                  write_lock_irqsave(&lock, flags);
                  thread_task->color = kcolors[i];
                  kretval[i] = 0;
                  write_unlock_irqrestore(&lock, flags);
            }
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
      if(copy_to_user(retval,kretval, sizeof(nr_pids)))
            return -EFAULT;
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

      printk("IN get_color\n");
      if(copy_from_user(kpids, pids, sizeof(nr_pids)))
            return -EFAULT;
      if(copy_from_user(kcolors, colors, sizeof(nr_pids)))
            return -EFAULT;

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
      if(copy_to_user(retval,kretval, sizeof(nr_pids)))
            return -EFAULT;
}
