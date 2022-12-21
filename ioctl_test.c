#include <stdio.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>

int main(void)
{

   int myi2cdev = open("/dev/myi2cdev", 0);  

   if (myi2cdev < 0)
   {			
      perror("Fail to open device file: /dev/myi2cdev.");
   } 
   else 
   {
      ioctl(myi2cdev, 100, 110); /* cmd = 100, arg = 110. */
      close(myi2cdev);
   }

   return 0;

}
