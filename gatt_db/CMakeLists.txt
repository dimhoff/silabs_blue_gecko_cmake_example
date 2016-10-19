#NOTE: Copy bgproj first for out-of-tree builds. because bgbuild generates
#      targets in same directory as source

#TODO: test with in-tree builds
add_custom_command(OUTPUT gatt_db.h gatt_db.c
	COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/gatt.bgproj .
	COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/gatt.xml .
	COMMAND ${GECKO_BGBUILD} -gn -c ${TOOLCHAIN_ROOT} gatt.bgproj
	MAIN_DEPENDENCY gatt.bgproj
	DEPENDS gatt.xml
	COMMENT "Generate GATT definition")

add_custom_target(gatt ALL DEPENDS gatt_db.h gatt_db.c)

add_library(gatt_db gatt_db.h gatt_db.c)
