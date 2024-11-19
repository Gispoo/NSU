#include <stdio.h>
#include <stdlib.h>
#include <libusb-1.0/libusb.h>


void print_device_info(libusb_device *dev) {
    struct libusb_device_descriptor desc;
    int r = libusb_get_device_descriptor(dev, &desc);
    if (r < 0) {
        fprintf(stderr, "Failed to get device descriptor: %s\n", libusb_error_name(r));
        return;
    }


    printf("Device Class: %02x\n", desc.bDeviceClass);
    printf("Vendor ID: %04x\n", desc.idVendor);
    printf("Product ID: %04x\n", desc.idProduct);


    libusb_device_handle *handle;
    r = libusb_open(dev, &handle);
    if (r != LIBUSB_SUCCESS) {
        fprintf(stderr, "Failed to open device: %s\n", libusb_error_name(r));
        return;
    }


    if (desc.iSerialNumber > 0) {
        unsigned char serial[256];
        r = libusb_get_string_descriptor_ascii(handle, desc.iSerialNumber, serial, sizeof(serial));
        if (r >= 0) {
            printf("Serial Number: %s\n", serial);
        } else {
            fprintf(stderr, "Failed to get Serial Number: %s\n", libusb_error_name(r));
        }
    } else {
        printf("No Serial Number\n");
    }


    libusb_close(handle);
}


int main(void) {
    libusb_context *ctx = NULL;
    int r = libusb_init(&ctx);
    if (r < 0) {
        fprintf(stderr, "Failed to initialize libusb: %s\n", libusb_error_name(r));
        return -1;
    }


    libusb_device **devices;
    ssize_t cnt = libusb_get_device_list(ctx, &devices);
    if (cnt < 0) {
        fprintf(stderr, "Error getting device list: %s\n", libusb_error_name(cnt));
        libusb_exit(ctx);
        return -1;
    }


    for (ssize_t i = 0; i < cnt; i++) {
        print_device_info(devices[i]);
        printf("\n");
    }


    libusb_free_device_list(devices, 1);
    libusb_exit(ctx);
    return 0;
}
