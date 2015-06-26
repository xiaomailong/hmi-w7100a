PROJECT(hmi-w7100A)

SET(w7100_src
    delay.c
    lcd.c
    loopback.c
    main.c
    serial.c
    socket.c
    TCPIPCore.c
    wizmemcpy.c
    delay.h
    iinchip_conf.h
    lcd.h
    loopback.h
    serial.h
    socket.h
    TCPIPCore.h
    types.h
    W7100.h
    wizmemcpy.h
    )

ADD_EXECUTABLE(hmi-w7100A ${w7100_src} )

