.PHONY: all
all: apidae.iso

.PHONY: all-hdd
all-hdd: apidae.hdd

.PHONY: run
run: apidae.iso
	qemu-system-x86_64 -M q35 -m 2G -cdrom apidae.iso -boot d -debugcon stdio

.PHONY: run-uefi
run-uefi: ovmf-x64 apidae.iso
	qemu-system-x86_64 -M q35 -m 2G -bios ovmf-x64/OVMF.fd -cdrom apidae.iso -boot d -debugcon stdio

.PHONY: run-hdd
run-hdd: apidae.hdd
	qemu-system-x86_64 -M q35 -m 2G -hda apidae.hdd -debugcon stdio

.PHONY: run-hdd-uefi
run-hdd-uefi: ovmf-x64 apidae.hdd
	qemu-system-x86_64 -M q35 -m 2G -bios ovmf-x64/OVMF.fd -hda apidae.hdd -debugcon stdio

ovmf-x64:
	mkdir -p ovmf-x64
	cd ovmf-x64 && curl -o OVMF-X64.zip https://efi.akeo.ie/OVMF/OVMF-X64.zip && 7z x OVMF-X64.zip

limine:
	git clone https://github.com/limine-bootloader/limine.git --branch=v3.0-branch-binary --depth=1
	make -C limine

.PHONY: kernel
kernel:
	$(MAKE) -C src
	cp src/neon -r build/neon

apidae.iso: limine kernel
	rm -rf iso_root
	mkdir -p iso_root
	cp src/neon \
		limine.cfg limine/limine.sys limine/limine-cd.bin limine/limine-cd-efi.bin iso_root/
	xorriso -as mkisofs -b limine-cd.bin \
		-no-emul-boot -boot-load-size 4 -boot-info-table \
		--efi-boot limine-cd-efi.bin \
		-efi-boot-part --efi-boot-image --protective-msdos-label \
		iso_root -o apidae.iso
	limine/limine-deploy apidae.iso
	rm -rf iso_root
	cp apidae.iso -r build/apidae.iso

apidae.hdd: limine kernel
	rm -f apidae.hdd
	dd if=/dev/zero bs=1M count=0 seek=64 of=apidae.hdd
	parted -s apidae.hdd mklabel gpt
	parted -s apidae.hdd mkpart ESP fat32 2048s 100%
	parted -s apidae.hdd set 1 esp on
	limine/limine-deploy apidae.hdd
	sudo losetup -Pf --show apidae.hdd >loopback_dev
	sudo mkfs.fat -F 32 `cat loopback_dev`p1
	mkdir -p img_mount
	sudo mount `cat loopback_dev`p1 img_mount
	sudo mkdir -p img_mount/EFI/BOOT
	sudo cp -v src/neon limine.cfg limine/limine.sys img_mount/
	sudo cp -v limine/BOOTX64.EFI img_mount/EFI/BOOT/
	sync
	sudo umount img_mount
	sudo losetup -d `cat loopback_dev`
	rm -rf loopback_dev img_mount

.PHONY: clean
clean:
	rm -rf iso_root apidae.iso apidae.hdd
	$(MAKE) -C source clean

.PHONY: distclean
distclean: clean
	rm -rf limine ovmf-x64
	$(MAKE) -C source distclean