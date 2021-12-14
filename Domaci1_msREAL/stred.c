#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/cdev.h>
#include <linux/kdev_t.h>
#include <linux/uaccess.h>
#include <linux/errno.h>
#include <linux/device.h>
#include <linux/wait.h>
#include <linux/semaphore.h>
#define BUFF_SIZE 200
MODULE_LICENSE("Dual BSD/GPL");

dev_t my_dev_id;
static struct class *my_class;
static struct device *my_device;
static struct cdev *my_cdev;

//DECLARE_WAIT_QUEUE_HEAD(readQ);
DECLARE_WAIT_QUEUE_HEAD(writeQ);
struct semaphore sem;


char stred[100];
int last_pos = 0;
int endRead = 0;



int stred_open(struct inode *pinode, struct file *pfile);
int stred_close(struct inode *pinode, struct file *pfile);
ssize_t stred_read(struct file *pfile, char __user *buffer, size_t length, loff_t *offset);
ssize_t stred_write(struct file *pfile, const char __user *buffer, size_t length, loff_t *offset);

struct file_operations my_fops =
{
	.owner = THIS_MODULE,
	.open = stred_open,
	.read = stred_read,
	.write = stred_write,
	.release = stred_close,
};


int stred_open(struct inode *pinode, struct file *pfile) 
{
		printk(KERN_INFO "Succesfully opened stred\n");
		return 0;
}

int stred_close(struct inode *pinode, struct file *pfile) 
{
		printk(KERN_INFO "Succesfully closed stred\n");
		return 0;
}

ssize_t stred_read(struct file *pfile, char __user *buffer, size_t length, loff_t *offset) 
{
	int ret;
	char buff[BUFF_SIZE];
	long int len;
	if(last_pos==0)
	{
		printk(KERN_INFO "Stred is empty\n");
		endRead=1;
	}
	if (endRead)
	{
		endRead = 0;
		printk(KERN_INFO "Succesfully read from file\n");
		return 0;
	}	
	len = scnprintf(buff,BUFF_SIZE , "String je: %s\n", stred);
	ret = copy_to_user(buffer, buff, len);
	if(ret)
		return -EFAULT;
	endRead=1;
	return len;
}

ssize_t stred_write(struct file *pfile, const char __user *buffer, size_t length, loff_t *offset) 
{
	char buff[BUFF_SIZE];
	char str[BUFF_SIZE];
	int value;
	int ret;
	int i;
	int pos;
	int temp;
	char *p1,*p2,*p3;
	int len1,len2,len3;
	int brisanje;
	
	
	

	ret = copy_from_user(buff, buffer, length);
	if(ret)
		return -EFAULT;
	buff[length-1] = '\0';
	if(strstr(buff, "string=")== buff)
	{
		p1=buff+7;
		if (down_interruptible(&sem))
			return -ERESTARTSYS;
		for(pos=0;pos<100;pos++)
			stred[pos]=0;
		len1=strlen(p1);
		if(len1<=100)
		{
			strncpy(stred,p1,len1);
			last_pos=len1;
			printk(KERN_INFO "Succesfully wrote string - New string is: %s\n",stred);
			up(&sem);
			wake_up_interruptible(&writeQ);	
		}
		else
		{
			up(&sem);
			printk(KERN_WARNING "Input string is bigger than 100 characters long\n");
		}	
	}
	else if(strstr(buff, "clear")== buff)
	{
		if (down_interruptible(&sem))
			return -ERESTARTSYS;
		for(pos=0;pos<100;pos++)
			stred[pos]=0;
		last_pos=0;
		up(&sem);
		printk(KERN_INFO "Succesfully cleared string\n");
		wake_up_interruptible(&writeQ);	
	}
	else if(strstr(buff, "shrink")== buff)
	{
		if(down_interruptible(&sem))
				return -ERESTARTSYS;
		pos=0;
		temp=last_pos;
		while(pos<last_pos)
		{
			if(stred[pos]==' ')
			{
				i=pos;
				while(i<last_pos)
				{
					stred[i]=stred[i+1];
					i++;
				}
				last_pos--;
			}
			pos++;	
		}
		if(last_pos<temp)
		{
			printk(KERN_INFO "Succesfully shrinked string - New string is: %s\n",stred);
		}
		else
		{
			printk(KERN_INFO "No space characters in stred\n");
		}
		up(&sem);
		if(last_pos<100)
		{
				
			wake_up_interruptible(&writeQ);
		}
		else
		{
			printk(KERN_WARNING "String is full\n");
		}
	}
	else if(strstr(buff, "append=")== buff)
	{
		p2 = buff + 7;
		len2 = strlen(p2);
		if(down_interruptible(&sem))
			return -ERESTARTSYS;
		while(last_pos + len2 >99)
		{
			up(&sem);
			if(wait_event_interruptible(writeQ,(last_pos+len2 < 100)))
				return -ERESTARTSYS;
			if(down_interruptible(&sem))
				return -ERESTARTSYS;
		}
		strcat(stred, p2);
		last_pos=last_pos+len2;
		printk(KERN_INFO "Succesfully appended - New string is: %s\n",stred);	
		up(&sem);
	}
	else if(strstr(buff, "truncate=")== buff)
	{
		ret = sscanf(buff,"truncate=%d",&value);
		if(down_interruptible(&sem))
			return -ERESTARTSYS;
		if(ret==1)
		{
			if((value<last_pos)&&
				(value>0))
			{
				pos=last_pos-value;
					
				while(pos<last_pos)
				{
					stred[pos]=0;
					pos++;
				}
			last_pos=last_pos-value;
			printk(KERN_INFO "Succesfully truncated - New string is: %s\n",stred);	
			}
			else
			{
				printk(KERN_WARNING "Wrong number of characters to truncate \n");	
			}
		}
		else
		{
			
			printk(KERN_WARNING "Wrong command format\n");
		}
		up(&sem);
		if(last_pos<100)
		{
			
			wake_up_interruptible(&writeQ);
		}
		else
		{
			printk(KERN_WARNING "String is full\n"); 
		}	
	}
	else if(strstr(buff, "remove=")== buff)
	{
		ret = sscanf(buff,"remove=%s",str);
		brisanje=0;
		len3 = strlen(str);
		p3=strstr(stred,str);
		if(down_interruptible(&sem))
			return -ERESTARTSYS;
		while(p3)
		{
			strncpy(p3, p3 + len3, strlen(p3));
			p3=strstr(stred,str);
			brisanje++;
		}
		last_pos=last_pos-brisanje*len3;
		printk(KERN_INFO "Succesfully removed - New string is: %s\n",stred);	
		
		up(&sem);
		if(last_pos<100)
		{
			wake_up_interruptible(&writeQ);
		}
		else
		{
			printk(KERN_WARNING "String is full\n"); 
		}
	}
	else
	{
		printk(KERN_WARNING "Wrong command format\n");
	}
	printk(KERN_INFO "SIZE = %d\n",strlen(stred));
	return length;
}
	
static int __init stred_init(void)
{
   int ret = 0;
	int i=0;
	
	sema_init(&sem,1);

	//Initialize array
	for (i=0; i<100; i++)
		stred[i] = 0;

   ret = alloc_chrdev_region(&my_dev_id, 0, 1, "stred");
   if (ret){
      printk(KERN_ERR "failed to register char device\n");
      return ret;
   }
   printk(KERN_INFO "char device region allocated\n");

   my_class = class_create(THIS_MODULE, "stred_class");
   if (my_class == NULL){
      printk(KERN_ERR "failed to create class\n");
      goto fail_0;
   }
   printk(KERN_INFO "class created\n");
   
   my_device = device_create(my_class, NULL, my_dev_id, NULL, "stred");
   if (my_device == NULL){
      printk(KERN_ERR "failed to create device\n");
      goto fail_1;
   }
   printk(KERN_INFO "device created\n");

	my_cdev = cdev_alloc();	
	my_cdev->ops = &my_fops;
	my_cdev->owner = THIS_MODULE;
	ret = cdev_add(my_cdev, my_dev_id, 1);
	if (ret)
	{
      printk(KERN_ERR "failed to add cdev\n");
		goto fail_2;
	}
   printk(KERN_INFO "cdev added\n");
   printk(KERN_INFO "Hello world\n");

   return 0;

   fail_2:
      device_destroy(my_class, my_dev_id);
   fail_1:
      class_destroy(my_class);
   fail_0:
      unregister_chrdev_region(my_dev_id, 1);
   return -1;
}

static void __exit stred_exit(void)
{
   cdev_del(my_cdev);
   device_destroy(my_class, my_dev_id);
   class_destroy(my_class);
   unregister_chrdev_region(my_dev_id,1);
   printk(KERN_INFO "Goodbye, cruel world\n");
}


module_init(stred_init);
module_exit(stred_exit);
