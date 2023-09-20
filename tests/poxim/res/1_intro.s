// Segmento de codigo
.text
    // Inicializacao
    init:
        bun main
        .align 5
    // Funcao principal
    main:
        // R1 = 12345
        mov r1, 12345
        // R10 = MEM[0x0000]
        l32 r10, [0x0000]
        // Finalizacao de execucao
        int 0