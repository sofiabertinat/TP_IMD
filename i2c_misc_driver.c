#include <linux/module.h>

#include <linux/fs.h>

#include <linux/miscdevice.h>

#include <linux/kernel.h>

#include <linux/init.h>

#include <linux/i2c.h>

#include <linux/types.h>



/******************************* 

    Declaracion funciones

********************************/

static int my_i2c_probe (struct i2c_client *client, const struct i2c_device_id *id);

static int my_i2c_remove(struct i2c_client *client);

static int my_dev_open(struct inode *inode, struct file *file);

static int my_dev_close(struct inode *inode, struct file *file);

static long my_dev_ioctl(struct file *file, unsigned int cmd, unsigned long arg);

/*********************

    Constantes

**********************/

/* 

*  PASO 1: Se declara IDs soportados por el driver (mediante of_device_id). 

* compatible: Es el string que se utiliza para hacer el match con el dispositivo descrito en el DT.

* data: Este puntero puede apuntar a cualquier estructura. Se utiliza para datos de configuración del device.

*/

static const struct of_device_id i2c_dt_ids[] = 

{

    { .compatible = "myi2c", },

    { /* sentinel */ }

};



/**********************

     Tipos datos

***********************/



static struct file_operations my_dev_fops = 

{

	.owner = THIS_MODULE,

        .open = my_dev_open,

        .release = my_dev_close,

        .unlocked_ioctl = my_dev_ioctl,

};



/* declacion & initializacion struct miscdevice */

static struct miscdevice i2c_miscdevice = 

{		

        .minor = MISC_DYNAMIC_MINOR,

        .name = "myi2cdev",

        .fops = &my_dev_fops,

};



/*  PASO 4: Declarar y llenar la estructura i2c_driver. */

struct i2c_driver my_i2c_driver =

{      

    	.probe = my_i2c_probe,

	.remove = my_i2c_remove,

    	/* Se pasa el array de IDs al campo of_match_table de la estructura driver */

	.driver =

	{

		.name = "i2c_driver",

		.owner = THIS_MODULE,

        	.of_match_table = i2c_dt_ids,

	},

};



struct my_i2c_device 

{

    struct i2c_client *client;

    struct i2c_driver *i2c_driver;

};



static struct my_i2c_device i2c_device = 

{

    .client = NULL,

    .i2c_driver = &my_i2c_driver,

};





/************************************

   PASO 3:   Funciones Driver

************************************/





/* 

* Se utiliza probe() para registrar el device mediante el framework misc.

* Mediante el framework misc se instancia el dispositivo y se le asignan las file operations correspondientes.

* Cada file operation hará uso de las API del I2C Subsystem para leer/escribir archivos. 

*/

static int my_i2c_probe (struct i2c_client *client, const struct i2c_device_id *id)

{	

    int ret_val = 0;  



    /* Register the device with the kernel */

    ret_val = misc_register(&i2c_miscdevice);



    if (ret_val != 0)

    {

        pr_err(" Could not register the misc device myi2c");

	    return ret_val;

    }

    else

    {

        pr_info(" The misc device myi2c was registered \n");

    }



    i2c_device.client = client;



    return ret_val;

}



/* 

* La funcion remove() deshace todo lo que hizo probe().

*/

static int my_i2c_remove(struct i2c_client *client)

{

    /* Unregistering from Kernel */

	misc_deregister(&i2c_miscdevice);

	pr_info(" The misc device myi2c was removed \n");

	return 0;

}





/**************************************

           Application I2C

 *************************************/



static int my_dev_open(struct inode *inode, struct file *file)

{

        pr_info("my_dev_open() is called.\n");

        return 0;

}



static int my_dev_close(struct inode *inode, struct file *file)  

{

        pr_info("my_dev_close() is called.\n");

        return 0;

}



static int my_dev_read( unsigned int reg, uint8_t *value)

{

	uint8_t send_buf[2], recv_buf[3];

	struct i2c_msg msgs[2];

	int ret = 0;

	

	pr_info("my_dev_read() is called.\n");

	

	send_buf[0] = reg;

	

	msgs[0].addr = i2c_device.client->addr;

	msgs[0].len = sizeof(send_buf);

	msgs[0].buf = send_buf; // pre-filled

	msgs[0].flags = 0; // Write transaction by default

	msgs[1].addr = i2c_device.client->addr;

	msgs[1].len = sizeof(recv_buf);

	msgs[1].buf = recv_buf;

	msgs[1].flags = I2C_M_RD; // Read transaction

	

	ret = i2c_transfer(i2c_device.client->adapter, msgs, ARRAY_SIZE(msgs));

	

	if (ret < 0) return ret;

	

	pr_info("recv_buf : %d%d%d \n", recv_buf[0], recv_buf[1], recv_buf[2]);

	

	*value = recv_buf[0];

	

	return ret;

}



static int my_dev_write(unsigned int reg, uint8_t *value)

{

	uint8_t send_buf[2], write_buf[3];

	struct i2c_msg msgs[2];

	int ret = 0;

	

	pr_info("my_dev_write() is called.\n");

	

	send_buf[0] = reg;

	write_buf[0] = *value;

	

	msgs[0].addr = i2c_device.client->addr;

	msgs[0].len = sizeof(send_buf);

	msgs[0].buf = send_buf; // pre-filled

	msgs[0].flags = 0; // Write transaction by default

	msgs[1].addr = i2c_device.client->addr;

	msgs[1].len = sizeof(write_buf);

	msgs[1].buf = write_buf;

	msgs[1].flags = 0;

	

	ret = i2c_transfer(i2c_device.client->adapter, msgs, ARRAY_SIZE(msgs));

	

	if (ret < 0) return ret;

	

	pr_info("register succesfully wrote \n");

	

	return ret;

}



static long my_dev_ioctl(struct file *file, unsigned int cmd, unsigned long arg)

{

	uint8_t value = 0;

	unsigned int reg = 0;

	

        pr_info("my_dev_ioctl() is called. cmd = %d, arg = %ld\n", cmd, arg);

        

        my_dev_read( reg, &value);

        

        pr_info("Read register: %d\n", value);

        

        value++;

        

        my_dev_write( reg, &value);        

        

        return 0;

}



/*************************



***************************/



/* PASO 2: Invocar a MODULE_DEVICE_TABLE(of, my_of_match_table ) para exponer el dispositivo.*/

MODULE_DEVICE_TABLE(of, i2c_dt_ids );



/* PASO 5: Utilizar la macro module_i2c_driver() para exponer el driver al kernel, pasando como argumento la estructura i2c_driver declarada antes.*/

module_i2c_driver(my_i2c_driver);



MODULE_LICENSE("GPL");

MODULE_AUTHOR("Sofia Bertinat ");

MODULE_DESCRIPTION("This is the i2c_driver using misc framework");