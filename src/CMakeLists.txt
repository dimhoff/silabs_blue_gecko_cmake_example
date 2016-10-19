add_executable(BLE_device.bin InitDevice.c main.c)
target_link_libraries(BLE_device.bin emdrv emlib gatt_db)
#add_dependencies(BLE_device.elf git_version)

add_custom_command(TARGET BLE_device.bin
	POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E rename BLE_device.bin BLE_device.elf
	COMMAND ${CMAKE_OBJCOPY} BLE_device.elf BLE_device.bin --gap-fill 255 -O binary 
	BYPRODUCTS BLE_device.elf
	)

add_custom_TARGET(load
	COMMAND ${BLE_SDK_ROOT}/protocol/bluetooth_2.0/bin/commander/commander flash BLE_device.bin
	DEPENDS BLE_device.bin
	COMMENT "Loading firmware to target")

