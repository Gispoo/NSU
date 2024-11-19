#include <iostream>
#include <libusb.h>
#include <stdio.h>
#include <string>

using namespace std;

void printdev(libusb_device *dev);

int main() {
    libusb_device **devs; 
    libusb_context *ctx = NULL;
    int r;
    ssize_t cnt;
    ssize_t i;

    r = libusb_init(&ctx);
    if (r < 0) {
        fprintf(stderr, "Error: Initialization failed, code: %d.n", r);
        return 1;
    }

    libusb_set_debug(ctx, 3);

    cnt = libusb_get_device_list(ctx, &devs);
    if (cnt < 0) {
        fprintf(stderr, "Error: USB device list not obtained.n", r);
        return 1;
    }

    printf("Найдено устройств: %dn", cnt);
    printf("=============================================================n");
    printf("* Количество возможных конфигурацийn");
    printf("| * Класс устройстваn");
    printf("| | * Идентификатор производителяn");
    printf("| | | * Идентификатор устройстваn");
    printf("| | | | * Серийный номерn");
    printf("| | | | | * Количество интерфейсовn");
    printf("| | | | | | * Количество альтернативных настроекn");
    printf("| | | | | | | * Класс устройстваn");
    printf("| | | | | | | | * Номер интерфейсаn");
    printf("| | | | | | | | | * Количество конечных точекn");
    printf("| | | | | | | | | | * Тип дескриптораn");
    printf("| | | | | | | | | | | * Адрес конечной точкиn");
    printf("+--+--+----+----+---+--+--+--+--+--+----------------------n");

    for (i = 0; i < cnt; i++) {
        printdev(devs[i]);
    }

    printf("=============================================================n");
    libusb_free_device_list(devs, 1);
    libusb_exit(ctx);

    return 0;
}

void printdev(libusb_device *dev) {
    libusb_device_descriptor desc;
    libusb_config_descriptor *config;
    const libusb_interface *inter;
    const libusb_interface_descriptor *interdesc;
    const libusb_endpoint_descriptor *epdesc;
    int r;

    r = libusb_get_device_descriptor(dev, &desc);
    if (r < 0) {
        fprintf(stderr, "Error: Device descriptor not obtained, code: %d.n", r);
        return;
    }

    libusb_get_config_descriptor(dev, 0, &config);

    libusb_device_handle *handle;
    r = libusb_open(dev, &handle);
    if (r < 0) {
        fprintf(stderr, "Error: Failed to open device.n");
        return;
    }

    unsigned char serial_string[256];
    int serial_length;
    r = libusb_get_string_descriptor_ascii(handle, desc.iSerialNumber, serial_string, sizeof(serial_string));
    if (r > 0) {
        serial_length = r;
    } else {
        serial_length = 0;
        strcpy((char*)serial_string, "N/A");
    }

    libusb_close(handle);

    printf("%.2d %.2d %.4d %.4d %.10s | | | | | |n",
           (int)desc.bNumConfigurations,
           (int)desc.bDeviceClass,
           desc.idVendor,
           desc.idProduct,
           serial_string
           );

    for (int i = 0; i < (int)config->bNumInterfaces; i++) {
        inter = &config->interface[i];
        printf("| | | | | %.2d %.2d | | | |n",
               inter->num_altsetting,
               (int)desc.bDeviceClass
               );
        for (int j = 0; j < inter->num_altsetting; j++) {
            interdesc = &inter->altsetting[j];
            printf("| | | | | | | %.2d %.2d | |n",
                   (int)interdesc->bInterfaceNumber,
                   (int)interdesc->bNumEndpoints
                   );
            for (int k = 0; k < (int)interdesc->bNumEndpoints; k++) {
                epdesc = &interdesc->endpoint[k];
                printf("| | | | | | | | | %.2d %.9dn",
                       (int)epdesc->bDescriptorType,
                       (int)(int)epdesc->bEndpointAddress
                       );
            }
        }
    }
    libusb_free_config_descriptor(config);
}
