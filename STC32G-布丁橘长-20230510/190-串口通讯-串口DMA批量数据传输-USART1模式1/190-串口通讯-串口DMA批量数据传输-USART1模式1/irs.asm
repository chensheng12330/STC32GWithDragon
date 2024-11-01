
        CSEG    AT  012BH           ;P0INT_VECTOR
        JMP     P0INT_ISR
        CSEG    AT  0133H           ;P1INT_VECTOR
        JMP     P1INT_ISR
        CSEG    AT  013BH           ;P2INT_VECTOR
        JMP     P2INT_ISR
        CSEG    AT  0143H           ;P3INT_VECTOR
        JMP     P3INT_ISR
        CSEG    AT  014BH           ;P4INT_VECTOR
        JMP     P4INT_ISR
        CSEG    AT  0153H           ;P5INT_VECTOR
        JMP     P5INT_ISR
        CSEG    AT  015BH           ;P6INT_VECTOR
        JMP     P6INT_ISR
        CSEG    AT  0163H           ;P7INT_VECTOR
        JMP     P7INT_ISR
        CSEG    AT  016BH           ;P8INT_VECTOR
        JMP     P8INT_ISR
        CSEG    AT  0173H           ;P9INT_VECTOR
        JMP     P9INT_ISR
        CSEG    AT  017BH           ;M2MBMM_VECTOR
        JMP     M2MBMM_ISR
        CSEG    AT  0183H           ;ADCBMM_VECTOR
        JMP     ADCBMM_ISR
        CSEG    AT  018BH           ;SPIBMM_VECTOR
        JMP     SPIBMM_ISR
        CSEG    AT  0193H           ;U1TXBMM_VECTOR
        JMP     U1TXBMM_ISR
        CSEG    AT  019BH           ;U1RXBMM_VECTOR
        JMP     U1RXBMM_ISR
        CSEG    AT  01A3H           ;U2TXBMM_VECTOR
        JMP     U2TXBMM_ISR
        CSEG    AT  01ABH           ;U2RXBMM_VECTOR
        JMP     U2RXBMM_ISR
        CSEG    AT  01B3H           ;U3TXBMM_VECTOR
        JMP     U3TXBMM_ISR
        CSEG    AT  01BBH           ;U3RXBMM_VECTOR
        JMP     U3RXBMM_ISR
        CSEG    AT  01C3H           ;U4TXBMM_VECTOR
        JMP     U4TXBMM_ISR
        CSEG    AT  01CBH           ;U4RXBMM_VECTOR
        JMP     U4RXBMM_ISR
        CSEG    AT  01D3H           ;LCMBMM_VECTOR
        JMP     LCMBMM_ISR
        CSEG    AT  01DBH           ;LCMIF_VECTOR
        JMP     LCMIF_ISR

P0INT_ISR:
P1INT_ISR:
P2INT_ISR:
P3INT_ISR:
P4INT_ISR:
P5INT_ISR:
P6INT_ISR:
P7INT_ISR:
P8INT_ISR:
P9INT_ISR:
M2MBMM_ISR:
ADCBMM_ISR:
SPIBMM_ISR:
U1TXBMM_ISR:
U1RXBMM_ISR:
U2TXBMM_ISR:
U2RXBMM_ISR:
U3TXBMM_ISR:
U3RXBMM_ISR:
U4TXBMM_ISR:
U4RXBMM_ISR:
LCMBMM_ISR:
LCMIF_ISR:

        JMP     006BH

        END
