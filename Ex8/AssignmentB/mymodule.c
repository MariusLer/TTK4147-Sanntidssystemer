#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>

int procfile_read(char *buffer, char **buffer_location, off_t offset, int buffer_length, int *eof, void *data){
	if (offset > 0){
		return 0;
	}
	else{
		return sprintf(buffer, "Hello world\n");
	}
}

int init_module(void){
   printk("Hello world 1.\n");
   struct proc_dir_entry* entry;
   entry = create_proc_entry("myproc", 0644, NULL);
   entry->read_proc = procfile_read;
   return 0;
}

void cleanup_module(void){
  printk(KERN_ALERT "Goodbye world 1.\n");
  remove_proc_entry("myproc", NULL);
}