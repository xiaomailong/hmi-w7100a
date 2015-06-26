PROJECT(hmi-w7100A)

SET(w7100_src
    ./Drivers/delay.c
    ./Drivers/iinchip_conf.h
    ./Drivers/serial.h
    ./Drivers/socket.h
    ./Drivers/TCPIPCore.c
    ./Drivers/types.h
    ./Drivers/W7100.h
    ./Drivers/wizmemcpy.h
    ./Drivers/delay.h
    ./Drivers/serial.c
    ./Drivers/socket.c
    ./Drivers/STARTUP.A51
    ./Drivers/TCPIPCore.h
    ./Drivers/wizmemcpy.c

    ./src/lcd.c
    ./src/lcd.h
    ./src/loopback.c
    ./src/loopback.h
    ./src/main.c
    )

ADD_EXECUTABLE(hmi-w7100A ${w7100_src} )

