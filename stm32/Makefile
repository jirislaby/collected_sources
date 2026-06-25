CC=arm-none-eabi-gcc-15
OBJCOPY=arm-none-eabi-objcopy
CFLAGS=-nostartfiles -fno-common -ffunction-sections -fdata-sections -mcpu=cortex-m3 -mthumb -msoft-float -g -Os -DSTM32F1
LDFLAGS=-Wl,--start-group -lc -lgcc -lnosys -Wl,--end-group -T ld.stm32.basic
LDLIBS=-lopencm3_stm32f1

all: stm32.bin

stm32.bin: stm32
	$(OBJCOPY) -Obinary $< $@
