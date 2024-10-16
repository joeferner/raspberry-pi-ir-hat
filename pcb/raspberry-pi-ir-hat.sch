EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A 11000 8500
encoding utf-8
Sheet 1 1
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L power:GND #PWR018
U 1 1 5F5B70E7
P 8500 3700
F 0 "#PWR018" H 8500 3450 50  0001 C CNN
F 1 "GND" H 8500 3550 50  0000 C CNN
F 2 "" H 8500 3700 50  0001 C CNN
F 3 "" H 8500 3700 50  0001 C CNN
	1    8500 3700
	1    0    0    -1  
$EndComp
$Comp
L power:VCC #PWR019
U 1 1 5F5B72BB
P 9000 800
F 0 "#PWR019" H 9000 650 50  0001 C CNN
F 1 "VCC" H 9000 950 50  0000 C CNN
F 2 "" H 9000 800 50  0001 C CNN
F 3 "" H 9000 800 50  0001 C CNN
	1    9000 800 
	1    0    0    -1  
$EndComp
$Comp
L connector:Raspberry_Pi_2_3 J7
U 1 1 5F5B7757
P 8900 2200
F 0 "J7" H 8200 3450 50  0000 L BNN
F 1 "Raspberry_Pi_2_3" H 9300 950 50  0000 L TNN
F 2 "Connector_PinSocket_2.54mm:PinSocket_2x20_P2.54mm_Vertical" H 8900 2200 50  0001 C CNN
F 3 "https://www.raspberrypi.org/documentation/hardware/raspberrypi/schematics/rpi_SCH_3bplus_1p0_reduced.pdf" H 8900 2200 50  0001 C CNN
	1    8900 2200
	1    0    0    -1  
$EndComp
Wire Wire Line
	8500 3500 8500 3600
Wire Wire Line
	8600 3500 8600 3600
Wire Wire Line
	8600 3600 8500 3600
Connection ~ 8500 3600
Wire Wire Line
	8500 3600 8500 3700
Wire Wire Line
	8700 3500 8700 3600
Wire Wire Line
	8700 3600 8600 3600
Connection ~ 8600 3600
Wire Wire Line
	8800 3500 8800 3600
Wire Wire Line
	8800 3600 8700 3600
Connection ~ 8700 3600
Wire Wire Line
	8900 3500 8900 3600
Wire Wire Line
	8900 3600 8800 3600
Connection ~ 8800 3600
Wire Wire Line
	9000 3500 9000 3600
Connection ~ 8900 3600
Wire Wire Line
	9000 3600 8900 3600
Wire Wire Line
	9100 3500 9100 3600
Wire Wire Line
	9100 3600 9000 3600
Connection ~ 9000 3600
Wire Wire Line
	9200 3500 9200 3600
Wire Wire Line
	9200 3600 9100 3600
Connection ~ 9100 3600
Wire Wire Line
	9000 800  9000 900 
NoConn ~ 9100 900 
NoConn ~ 8800 900 
NoConn ~ 8100 2000
NoConn ~ 8100 2100
NoConn ~ 9700 3000
NoConn ~ 9700 2900
NoConn ~ 9700 2700
NoConn ~ 9700 2600
NoConn ~ 9700 2500
NoConn ~ 9700 2400
NoConn ~ 9700 2300
NoConn ~ 9700 2100
NoConn ~ 9700 2000
NoConn ~ 9700 1900
NoConn ~ 9700 1700
NoConn ~ 9700 1600
NoConn ~ 9700 1400
NoConn ~ 9700 1300
Wire Wire Line
	8100 2200 7500 2200
Text Label 7500 2200 0    50   ~ 0
RW
Text Label 5350 7050 2    50   ~ 0
RW
Wire Wire Line
	5350 7050 5150 7050
$Comp
L power:GND #PWR020
U 1 1 5F5D1A66
P 5350 7250
F 0 "#PWR020" H 5350 7000 50  0001 C CNN
F 1 "GND" H 5350 7100 50  0000 C CNN
F 2 "" H 5350 7250 50  0001 C CNN
F 3 "" H 5350 7250 50  0001 C CNN
	1    5350 7250
	1    0    0    -1  
$EndComp
Wire Wire Line
	5150 7150 5350 7150
Wire Wire Line
	5350 7150 5350 7250
$Comp
L connector_generic:Conn_02x03_Odd_Even J3
U 1 1 5F5D67BE
P 1500 4600
F 0 "J3" H 1550 4800 50  0000 C CNN
F 1 "RX_JMP" H 1550 4400 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_2x03_P2.54mm_Vertical" H 1500 4600 50  0001 C CNN
F 3 "~" H 1500 4600 50  0001 C CNN
	1    1500 4600
	1    0    0    1   
$EndComp
$Comp
L kicad-library:AudioJack2_Ground J1
U 1 1 5F5D8D83
P 850 4600
F 0 "J1" H 850 4950 50  0000 C CNN
F 1 "RX" H 850 4850 50  0000 C CNN
F 2 "kicad-library:CUIDEVICES-SJ1-3523NG" H 850 4600 50  0001 C CNN
F 3 "~" H 850 4600 50  0001 C CNN
	1    850  4600
	1    0    0    -1  
$EndComp
$Comp
L power:VCC #PWR03
U 1 1 5F5DE972
P 1900 4300
F 0 "#PWR03" H 1900 4150 50  0001 C CNN
F 1 "VCC" H 1900 4450 50  0000 C CNN
F 2 "" H 1900 4300 50  0001 C CNN
F 3 "" H 1900 4300 50  0001 C CNN
	1    1900 4300
	1    0    0    -1  
$EndComp
Wire Wire Line
	1800 4500 1900 4500
Wire Wire Line
	1900 4500 1900 4300
$Comp
L kicad-library:AudioJack2_Ground J2
U 1 1 5F5EEE5F
P 1050 2100
F 0 "J2" H 1050 2450 50  0000 C CNN
F 1 "TX" H 1050 2350 50  0000 C CNN
F 2 "kicad-library:CUIDEVICES-SJ1-3523NG" H 1050 2100 50  0001 C CNN
F 3 "~" H 1050 2100 50  0001 C CNN
	1    1050 2100
	1    0    0    -1  
$EndComp
$Comp
L connector_generic:Conn_02x02_Odd_Even J4
U 1 1 5F5F00A7
P 1750 2100
F 0 "J4" H 1800 2200 50  0000 C CNN
F 1 "TX_JMP" H 1800 1900 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_2x02_P2.54mm_Vertical" H 1750 2100 50  0001 C CNN
F 3 "~" H 1750 2100 50  0001 C CNN
	1    1750 2100
	1    0    0    1   
$EndComp
Wire Wire Line
	1550 2100 1450 2100
Wire Wire Line
	1450 2100 1450 2400
Wire Wire Line
	1450 2400 1050 2400
Wire Wire Line
	1050 2400 1050 2300
Wire Wire Line
	1250 2100 1350 2100
Wire Wire Line
	1350 2100 1350 2000
Wire Wire Line
	1350 2000 1550 2000
NoConn ~ 1250 2000
$Comp
L connector_generic:Conn_01x02 J8
U 1 1 5F5FFCAF
P 4950 7150
F 0 "J8" H 4950 7250 50  0000 C CNN
F 1 "RW_JMP" H 4950 6950 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x02_P2.54mm_Vertical" H 4950 7150 50  0001 C CNN
F 3 "~" H 4950 7150 50  0001 C CNN
	1    4950 7150
	-1   0    0    1   
$EndComp
$Comp
L device:R R1
U 1 1 5F60C3F9
P 2150 1650
F 0 "R1" V 2230 1650 50  0000 C CNN
F 1 "100" V 2150 1650 50  0000 C CNN
F 2 "Resistor_SMD:R_0805_2012Metric" V 2080 1650 50  0001 C CNN
F 3 "~" H 2150 1650 50  0001 C CNN
	1    2150 1650
	-1   0    0    1   
$EndComp
Wire Wire Line
	2650 1800 2650 1900
$Comp
L device:LED D2
U 1 1 5F5F7EB2
P 2650 2050
F 0 "D2" H 2650 2150 50  0000 C CNN
F 1 "TX" H 2650 1950 50  0000 C CNN
F 2 "LED_SMD:LED_0805_2012Metric" H 2650 2050 50  0001 C CNN
F 3 "~" H 2650 2050 50  0001 C CNN
	1    2650 2050
	0    -1   -1   0   
$EndComp
$Comp
L device:R R3
U 1 1 5F5F7A8A
P 2650 1650
F 0 "R3" V 2730 1650 50  0000 C CNN
F 1 "100" V 2650 1650 50  0000 C CNN
F 2 "Resistor_SMD:R_0805_2012Metric" V 2580 1650 50  0001 C CNN
F 3 "~" H 2650 1650 50  0001 C CNN
	1    2650 1650
	1    0    0    -1  
$EndComp
Text Label 6850 2750 2    50   ~ 0
IR_TX
Text Label 2250 4700 2    50   ~ 0
IR_RX
$Comp
L device:C C1
U 1 1 5F62EC94
P 1500 5400
F 0 "C1" H 1525 5500 50  0000 L CNN
F 1 "0.1uF" H 1525 5300 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 1538 5250 50  0001 C CNN
F 3 "~" H 1500 5400 50  0001 C CNN
	1    1500 5400
	1    0    0    -1  
$EndComp
$Comp
L power:VCC #PWR01
U 1 1 5F62F830
P 1500 5150
F 0 "#PWR01" H 1500 5000 50  0001 C CNN
F 1 "VCC" H 1500 5300 50  0000 C CNN
F 2 "" H 1500 5150 50  0001 C CNN
F 3 "" H 1500 5150 50  0001 C CNN
	1    1500 5150
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR02
U 1 1 5F631B94
P 1500 5650
F 0 "#PWR02" H 1500 5400 50  0001 C CNN
F 1 "GND" H 1500 5500 50  0000 C CNN
F 2 "" H 1500 5650 50  0001 C CNN
F 3 "" H 1500 5650 50  0001 C CNN
	1    1500 5650
	1    0    0    -1  
$EndComp
Wire Wire Line
	1500 5150 1500 5250
Wire Wire Line
	1500 5550 1500 5650
$Comp
L device:LED D1
U 1 1 5F6412BE
P 2500 5150
F 0 "D1" H 2500 5250 50  0000 C CNN
F 1 "RX" H 2500 5050 50  0000 C CNN
F 2 "LED_SMD:LED_0805_2012Metric" H 2500 5150 50  0001 C CNN
F 3 "~" H 2500 5150 50  0001 C CNN
	1    2500 5150
	0    -1   -1   0   
$EndComp
$Comp
L device:R R2
U 1 1 5F641AA8
P 2500 4750
F 0 "R2" V 2580 4750 50  0000 C CNN
F 1 "100" V 2500 4750 50  0000 C CNN
F 2 "Resistor_SMD:R_0805_2012Metric" V 2430 4750 50  0001 C CNN
F 3 "~" H 2500 4750 50  0001 C CNN
	1    2500 4750
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR07
U 1 1 5F6420B8
P 2500 5400
F 0 "#PWR07" H 2500 5150 50  0001 C CNN
F 1 "GND" H 2500 5250 50  0000 C CNN
F 2 "" H 2500 5400 50  0001 C CNN
F 3 "" H 2500 5400 50  0001 C CNN
	1    2500 5400
	1    0    0    -1  
$EndComp
Text Label 2750 4500 2    50   ~ 0
RXLED
Wire Wire Line
	2750 4500 2500 4500
Wire Wire Line
	2500 4500 2500 4600
Wire Wire Line
	2500 4900 2500 5000
Wire Wire Line
	2500 5300 2500 5400
$Comp
L connector_generic:Conn_01x06 J6
U 1 1 5F64A133
P 4100 7000
F 0 "J6" H 4100 7300 50  0000 C CNN
F 1 "IO" H 4100 6600 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x06_P2.54mm_Vertical" H 4100 7000 50  0001 C CNN
F 3 "~" H 4100 7000 50  0001 C CNN
	1    4100 7000
	1    0    0    -1  
$EndComp
Wire Wire Line
	8100 2400 7500 2400
Text Label 7500 2400 0    50   ~ 0
IORP1
Text Label 6850 1750 2    50   ~ 0
CUR2
Text Label 6850 1650 2    50   ~ 0
CUR1
Text Label 3650 7000 0    50   ~ 0
IORP1
Wire Wire Line
	3900 6800 3650 6800
Wire Wire Line
	3650 6900 3900 6900
Wire Wire Line
	3900 7000 3650 7000
Wire Wire Line
	3650 7100 3900 7100
Wire Wire Line
	1050 4600 1200 4600
Wire Wire Line
	1200 4600 1200 4500
Wire Wire Line
	1200 4500 1300 4500
Wire Wire Line
	850  4800 850  4850
Wire Wire Line
	850  4850 1250 4850
Wire Wire Line
	1250 4850 1250 4600
Wire Wire Line
	1250 4600 1300 4600
Wire Wire Line
	1300 4700 1150 4700
Wire Wire Line
	1150 4700 1150 4500
Wire Wire Line
	1150 4500 1050 4500
NoConn ~ 8100 2700
NoConn ~ 8100 2800
$Comp
L power:GND #PWR017
U 1 1 5F6C9223
P 3800 7400
F 0 "#PWR017" H 3800 7150 50  0001 C CNN
F 1 "GND" H 3800 7250 50  0000 C CNN
F 2 "" H 3800 7400 50  0001 C CNN
F 3 "" H 3800 7400 50  0001 C CNN
	1    3800 7400
	1    0    0    -1  
$EndComp
$Comp
L power:VCC #PWR016
U 1 1 5F6CA428
P 3350 7100
F 0 "#PWR016" H 3350 6950 50  0001 C CNN
F 1 "VCC" H 3350 7250 50  0000 C CNN
F 2 "" H 3350 7100 50  0001 C CNN
F 3 "" H 3350 7100 50  0001 C CNN
	1    3350 7100
	1    0    0    -1  
$EndComp
Wire Wire Line
	3900 7200 3350 7200
Wire Wire Line
	3350 7200 3350 7100
Wire Wire Line
	3900 7300 3800 7300
Wire Wire Line
	3800 7300 3800 7400
Text Label 8700 700  3    50   ~ 0
5V
Wire Wire Line
	8700 900  8700 700 
Text Label 3650 7100 0    50   ~ 0
5V
$Comp
L device:LED D3
U 1 1 5F74286A
P 1100 7200
F 0 "D3" H 1100 7300 50  0000 C CNN
F 1 "POWER" H 1100 7100 50  0000 C CNN
F 2 "LED_SMD:LED_0805_2012Metric" H 1100 7200 50  0001 C CNN
F 3 "~" H 1100 7200 50  0001 C CNN
	1    1100 7200
	0    -1   -1   0   
$EndComp
$Comp
L device:R R4
U 1 1 5F742870
P 1100 6800
F 0 "R4" V 1180 6800 50  0000 C CNN
F 1 "100" V 1100 6800 50  0000 C CNN
F 2 "Resistor_SMD:R_0805_2012Metric" V 1030 6800 50  0001 C CNN
F 3 "~" H 1100 6800 50  0001 C CNN
	1    1100 6800
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR09
U 1 1 5F742876
P 1100 7450
F 0 "#PWR09" H 1100 7200 50  0001 C CNN
F 1 "GND" H 1100 7300 50  0000 C CNN
F 2 "" H 1100 7450 50  0001 C CNN
F 3 "" H 1100 7450 50  0001 C CNN
	1    1100 7450
	1    0    0    -1  
$EndComp
Wire Wire Line
	1100 6550 1100 6650
Wire Wire Line
	1100 6950 1100 7050
Wire Wire Line
	1100 7350 1100 7450
$Comp
L power:VCC #PWR08
U 1 1 5F744D15
P 1100 6550
F 0 "#PWR08" H 1100 6400 50  0001 C CNN
F 1 "VCC" H 1100 6700 50  0000 C CNN
F 2 "" H 1100 6550 50  0001 C CNN
F 3 "" H 1100 6550 50  0001 C CNN
	1    1100 6550
	1    0    0    -1  
$EndComp
Text Notes 650  4000 0    50   ~ 0
RECEIVE
Text Notes 650  1000 0    50   ~ 0
TRANSMIT
$Comp
L device:Q_NMOS_GSD Q1
U 1 1 5F751800
P 2550 2750
F 0 "Q1" H 2755 2796 50  0000 L CNN
F 1 "Q_NMOS_GSD" V 2800 2250 50  0000 L CNN
F 2 "Package_TO_SOT_SMD:TSOT-23" H 2750 2850 50  0001 C CNN
F 3 "~" H 2550 2750 50  0001 C CNN
	1    2550 2750
	-1   0    0    -1  
$EndComp
$Comp
L power:GND #PWR06
U 1 1 5F755F74
P 2450 3300
F 0 "#PWR06" H 2450 3050 50  0001 C CNN
F 1 "GND" H 2450 3150 50  0000 C CNN
F 2 "" H 2450 3300 50  0001 C CNN
F 3 "" H 2450 3300 50  0001 C CNN
	1    2450 3300
	1    0    0    -1  
$EndComp
Wire Wire Line
	2450 2950 2450 3300
Wire Wire Line
	3000 2750 2750 2750
Wire Wire Line
	2150 1800 2150 2000
Wire Wire Line
	2150 2000 2050 2000
Wire Wire Line
	2050 2100 2200 2100
Wire Wire Line
	2200 2100 2200 2450
Wire Wire Line
	2200 2450 2450 2450
Wire Wire Line
	2450 2450 2450 2550
Wire Wire Line
	2650 2200 2650 2450
Wire Wire Line
	2650 2450 2450 2450
Connection ~ 2450 2450
$Comp
L power:VCC #PWR05
U 1 1 5F7741EC
P 2400 1300
F 0 "#PWR05" H 2400 1150 50  0001 C CNN
F 1 "VCC" H 2400 1450 50  0000 C CNN
F 2 "" H 2400 1300 50  0001 C CNN
F 3 "" H 2400 1300 50  0001 C CNN
	1    2400 1300
	1    0    0    -1  
$EndComp
Wire Wire Line
	2400 1300 2400 1400
Wire Wire Line
	2400 1400 2150 1400
Wire Wire Line
	2150 1400 2150 1500
Wire Wire Line
	2400 1400 2650 1400
Wire Wire Line
	2650 1400 2650 1500
Connection ~ 2400 1400
Wire Wire Line
	1800 4700 2250 4700
Wire Wire Line
	1900 4600 1900 4800
Wire Wire Line
	1800 4600 1900 4600
$Comp
L power:GND #PWR04
U 1 1 5F5DD51E
P 1900 4800
F 0 "#PWR04" H 1900 4550 50  0001 C CNN
F 1 "GND" H 1900 4650 50  0000 C CNN
F 2 "" H 1900 4800 50  0001 C CNN
F 3 "" H 1900 4800 50  0001 C CNN
	1    1900 4800
	1    0    0    -1  
$EndComp
$Comp
L kicad-library:STM32G030Fx_TSSOP20 U1
U 1 1 5F981B79
P 5400 2050
F 0 "U1" H 4650 3000 50  0000 L CNN
F 1 "STM32G030Fx_TSSOP20" H 4650 1100 50  0000 L CNN
F 2 "Housings_SSOP:SSOP-20_4.4x6.5mm_Pitch0.65mm" H 4850 1650 50  0001 C CNN
F 3 "" H 4850 1650 50  0001 C CNN
	1    5400 2050
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR013
U 1 1 5F9C8642
P 4350 2950
F 0 "#PWR013" H 4350 2700 50  0001 C CNN
F 1 "GND" H 4350 2800 50  0000 C CNN
F 2 "" H 4350 2950 50  0001 C CNN
F 3 "" H 4350 2950 50  0001 C CNN
	1    4350 2950
	1    0    0    -1  
$EndComp
$Comp
L power:VCC #PWR012
U 1 1 5F9CB5CA
P 4350 1150
F 0 "#PWR012" H 4350 1000 50  0001 C CNN
F 1 "VCC" H 4350 1300 50  0000 C CNN
F 2 "" H 4350 1150 50  0001 C CNN
F 3 "" H 4350 1150 50  0001 C CNN
	1    4350 1150
	1    0    0    -1  
$EndComp
Wire Wire Line
	4450 1250 4350 1250
Wire Wire Line
	4350 1250 4350 1150
Wire Wire Line
	4450 2850 4350 2850
Wire Wire Line
	4350 2850 4350 2950
$Comp
L device:C C2
U 1 1 5F9D0EDA
P 4000 1750
F 0 "C2" H 4025 1850 50  0000 L CNN
F 1 "0.1uF" H 4025 1650 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 4038 1600 50  0001 C CNN
F 3 "~" H 4000 1750 50  0001 C CNN
	1    4000 1750
	1    0    0    -1  
$EndComp
$Comp
L power:VCC #PWR010
U 1 1 5F9D1B88
P 4000 1500
F 0 "#PWR010" H 4000 1350 50  0001 C CNN
F 1 "VCC" H 4000 1650 50  0000 C CNN
F 2 "" H 4000 1500 50  0001 C CNN
F 3 "" H 4000 1500 50  0001 C CNN
	1    4000 1500
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR011
U 1 1 5F9D1FF7
P 4000 2000
F 0 "#PWR011" H 4000 1750 50  0001 C CNN
F 1 "GND" H 4000 1850 50  0000 C CNN
F 2 "" H 4000 2000 50  0001 C CNN
F 3 "" H 4000 2000 50  0001 C CNN
	1    4000 2000
	1    0    0    -1  
$EndComp
Wire Wire Line
	4000 1600 4000 1500
Wire Wire Line
	4000 1900 4000 2000
NoConn ~ 8100 2900
Text Label 7500 1400 0    50   ~ 0
RPI_USART_RX
Text Label 7500 1300 0    50   ~ 0
RPI_USART_TX
Text Label 6850 1550 2    50   ~ 0
RPI_USART_TX
Text Label 6850 1450 2    50   ~ 0
RPI_USART_RX
Wire Wire Line
	6850 2750 6250 2750
Wire Wire Line
	6850 1550 6250 1550
Wire Wire Line
	6850 1450 6250 1450
Wire Wire Line
	7500 1400 8100 1400
Wire Wire Line
	8100 1300 7500 1300
Text Label 7300 1700 0    50   ~ 0
MCU_RESET
Text Label 3950 2650 0    50   ~ 0
MCU_RESET
Wire Wire Line
	4450 2650 3950 2650
NoConn ~ 8100 1600
Text Label 7300 1800 0    50   ~ 0
MCU_BOOT0
Text Label 6850 2450 2    50   ~ 0
MCU_BOOT0
Wire Wire Line
	6850 2450 6250 2450
Text Label 3000 2750 2    50   ~ 0
IR_TX
Text Label 6850 1850 2    50   ~ 0
IR_RX
Wire Wire Line
	6850 1850 6250 1850
Text Label 6850 1950 2    50   ~ 0
RXLED
Wire Wire Line
	6850 1950 6250 1950
Text Label 4350 4250 0    50   ~ 0
MCU_RESET
Wire Wire Line
	4850 4250 4350 4250
$Comp
L power:GND #PWR015
U 1 1 5FA46C6A
P 4750 4950
F 0 "#PWR015" H 4750 4700 50  0001 C CNN
F 1 "GND" H 4750 4800 50  0000 C CNN
F 2 "" H 4750 4950 50  0001 C CNN
F 3 "" H 4750 4950 50  0001 C CNN
	1    4750 4950
	1    0    0    -1  
$EndComp
$Comp
L power:VCC #PWR014
U 1 1 5FA4D097
P 4750 3750
F 0 "#PWR014" H 4750 3600 50  0001 C CNN
F 1 "VCC" H 4750 3900 50  0000 C CNN
F 2 "" H 4750 3750 50  0001 C CNN
F 3 "" H 4750 3750 50  0001 C CNN
	1    4750 3750
	1    0    0    -1  
$EndComp
Text Label 4350 4050 0    50   ~ 0
MCU_BOOT0
Wire Wire Line
	4850 4050 4350 4050
Text Label 6850 2350 2    50   ~ 0
SWDIO
Wire Wire Line
	6850 2350 6250 2350
Text Label 4350 4150 0    50   ~ 0
SWDIO
Wire Wire Line
	4850 4150 4350 4150
Wire Wire Line
	4750 3750 4750 3850
Wire Wire Line
	4750 3850 4850 3850
Wire Notes Line style solid
	3450 6000 3450 4000
Wire Notes Line style solid
	650  4000 650  6000
Wire Notes Line style solid
	3450 1000 3450 3650
Wire Notes Line style solid
	3450 3650 650  3650
Wire Notes Line style solid
	650  3650 650  1000
Wire Notes Line style solid
	650  1000 3450 1000
NoConn ~ 6250 2850
NoConn ~ 6250 2250
NoConn ~ 6250 2050
Wire Notes Line style solid
	650  6000 3450 6000
Wire Notes Line style solid
	3450 4000 650  4000
$Comp
L kicad-library:ST_PROGRAM_DEBUG P1
U 1 1 5F9982B3
P 5050 4350
F 0 "P1" H 5050 5000 60  0000 L CNN
F 1 "ST_PROGRAM_DEBUG" H 5050 3700 60  0000 L CNN
F 2 "kicad-library:PinHeader_2x04_P2.54mm_Vertical_Shrouded" H 4750 4150 60  0001 C CNN
F 3 "" H 4750 4150 60  0000 C CNN
	1    5050 4350
	1    0    0    -1  
$EndComp
NoConn ~ 4850 4350
Wire Wire Line
	4850 4850 4750 4850
Wire Wire Line
	4750 4850 4750 4950
Text Label 4350 4650 0    50   ~ 0
DEBUG_TX
Text Label 4350 4550 0    50   ~ 0
DEBUG_RX
Wire Wire Line
	4350 4550 4850 4550
Wire Wire Line
	4850 4650 4350 4650
Text Label 6850 2650 2    50   ~ 0
DEBUG_RX
Text Label 6850 2550 2    50   ~ 0
DEBUG_TX
Wire Wire Line
	6850 2550 6250 2550
Wire Wire Line
	6250 2650 6850 2650
$Comp
L kicad-library:PWR_FLAG #FLG0101
U 1 1 5F990BF5
P 2000 7050
F 0 "#FLG0101" H 2000 7145 30  0001 C CNN
F 1 "PWR_FLAG" H 2000 7173 30  0000 C CNN
F 2 "" H 2000 7050 60  0000 C CNN
F 3 "" H 2000 7050 60  0000 C CNN
	1    2000 7050
	1    0    0    -1  
$EndComp
$Comp
L kicad-library:PWR_FLAG #FLG0102
U 1 1 5F9923BD
P 2300 7150
F 0 "#FLG0102" H 2300 7245 30  0001 C CNN
F 1 "PWR_FLAG" H 2300 7272 30  0000 C CNN
F 2 "" H 2300 7150 60  0000 C CNN
F 3 "" H 2300 7150 60  0000 C CNN
	1    2300 7150
	-1   0    0    1   
$EndComp
$Comp
L kicad-library:PWR_FLAG #FLG0103
U 1 1 5F992687
P 2600 7150
F 0 "#FLG0103" H 2600 7245 30  0001 C CNN
F 1 "PWR_FLAG" H 2600 7272 30  0000 C CNN
F 2 "" H 2600 7150 60  0000 C CNN
F 3 "" H 2600 7150 60  0000 C CNN
	1    2600 7150
	-1   0    0    1   
$EndComp
$Comp
L power:GND #PWR0101
U 1 1 5F99290E
P 2000 7150
F 0 "#PWR0101" H 2000 6900 50  0001 C CNN
F 1 "GND" H 2000 7000 50  0000 C CNN
F 2 "" H 2000 7150 50  0001 C CNN
F 3 "" H 2000 7150 50  0001 C CNN
	1    2000 7150
	1    0    0    -1  
$EndComp
$Comp
L power:VCC #PWR0102
U 1 1 5F993000
P 2300 7050
F 0 "#PWR0102" H 2300 6900 50  0001 C CNN
F 1 "VCC" H 2300 7200 50  0000 C CNN
F 2 "" H 2300 7050 50  0001 C CNN
F 3 "" H 2300 7050 50  0001 C CNN
	1    2300 7050
	1    0    0    -1  
$EndComp
Text Label 2600 6950 3    50   ~ 0
5V
Wire Wire Line
	2000 7050 2000 7150
Wire Wire Line
	2300 7150 2300 7050
Wire Wire Line
	2600 7150 2600 6950
$Comp
L device:R R5
U 1 1 5F9AB55E
P 7900 1700
F 0 "R5" V 7800 1650 50  0000 C CNN
F 1 "10K" V 7900 1700 50  0000 C CNN
F 2 "Resistor_SMD:R_0805_2012Metric" V 7830 1700 50  0001 C CNN
F 3 "~" H 7900 1700 50  0001 C CNN
	1    7900 1700
	0    1    1    0   
$EndComp
$Comp
L device:R R6
U 1 1 5F9AC427
P 7900 1800
F 0 "R6" V 8000 1750 50  0000 C CNN
F 1 "10K" V 7900 1800 50  0000 C CNN
F 2 "Resistor_SMD:R_0805_2012Metric" V 7830 1800 50  0001 C CNN
F 3 "~" H 7900 1800 50  0001 C CNN
	1    7900 1800
	0    1    1    0   
$EndComp
Wire Wire Line
	8100 1700 8050 1700
Wire Wire Line
	8100 1800 8050 1800
Wire Wire Line
	7750 1800 7300 1800
Wire Wire Line
	7750 1700 7300 1700
NoConn ~ 8100 2500
NoConn ~ 8100 2600
Wire Wire Line
	6850 1650 6250 1650
Wire Wire Line
	6850 1750 6250 1750
$Comp
L device:R R7
U 1 1 61793E70
P 7950 4750
F 0 "R7" V 8030 4750 50  0000 C CNN
F 1 "DNP" V 7950 4750 50  0000 C CNN
F 2 "Resistor_SMD:R_0805_2012Metric" V 7880 4750 50  0001 C CNN
F 3 "~" H 7950 4750 50  0001 C CNN
	1    7950 4750
	1    0    0    -1  
$EndComp
$Comp
L kicad-library:AudioJack2_Ground J5
U 1 1 617A007F
P 7500 4500
F 0 "J5" H 7500 4850 50  0000 C CNN
F 1 "RX" H 7500 4750 50  0000 C CNN
F 2 "kicad-library:CUIDEVICES-SJ1-3523NG" H 7500 4500 50  0001 C CNN
F 3 "~" H 7500 4500 50  0001 C CNN
	1    7500 4500
	1    0    0    -1  
$EndComp
$Comp
L device:R R9
U 1 1 617B5FAA
P 9800 4750
F 0 "R9" V 9880 4750 50  0000 C CNN
F 1 "100K" V 9800 4750 50  0000 C CNN
F 2 "Resistor_SMD:R_0805_2012Metric" V 9730 4750 50  0001 C CNN
F 3 "~" H 9800 4750 50  0001 C CNN
	1    9800 4750
	1    0    0    -1  
$EndComp
$Comp
L device:R R10
U 1 1 617B9D60
P 9800 5400
F 0 "R10" V 9880 5400 50  0000 C CNN
F 1 "100K" V 9800 5400 50  0000 C CNN
F 2 "Resistor_SMD:R_0805_2012Metric" V 9730 5400 50  0001 C CNN
F 3 "~" H 9800 5400 50  0001 C CNN
	1    9800 5400
	1    0    0    -1  
$EndComp
$Comp
L device:C C3
U 1 1 617C52F0
P 9150 5350
F 0 "C3" H 9175 5450 50  0000 L CNN
F 1 "47uF" H 9175 5250 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 9188 5200 50  0001 C CNN
F 3 "~" H 9150 5350 50  0001 C CNN
	1    9150 5350
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR022
U 1 1 617CABF7
P 9800 5750
F 0 "#PWR022" H 9800 5500 50  0001 C CNN
F 1 "GND" H 9800 5600 50  0000 C CNN
F 2 "" H 9800 5750 50  0001 C CNN
F 3 "" H 9800 5750 50  0001 C CNN
	1    9800 5750
	1    0    0    -1  
$EndComp
Wire Wire Line
	9150 5650 9800 5650
Wire Wire Line
	9800 5650 9800 5550
Wire Wire Line
	9800 5650 9800 5750
Connection ~ 9800 5650
Wire Wire Line
	9800 5250 9800 5050
Wire Wire Line
	9800 5050 9150 5050
Connection ~ 9800 5050
Wire Wire Line
	9800 5050 9800 4900
Wire Wire Line
	7950 5050 7950 4900
Wire Wire Line
	9800 4600 9800 4500
$Comp
L power:VCC #PWR021
U 1 1 617E9691
P 9800 4500
F 0 "#PWR021" H 9800 4350 50  0001 C CNN
F 1 "VCC" H 9800 4650 50  0000 C CNN
F 2 "" H 9800 4500 50  0001 C CNN
F 3 "" H 9800 4500 50  0001 C CNN
	1    9800 4500
	1    0    0    -1  
$EndComp
Wire Wire Line
	7500 4700 7500 5050
Wire Wire Line
	7500 5050 7950 5050
Wire Wire Line
	7950 4500 7950 4600
Wire Wire Line
	7700 4500 7950 4500
NoConn ~ 7700 4400
Wire Wire Line
	7950 4500 7950 4350
Wire Wire Line
	7950 4350 8250 4350
Connection ~ 7950 4500
Text Label 8250 4350 2    50   ~ 0
CUR1
$Comp
L device:R R8
U 1 1 6184DC85
P 9150 4750
F 0 "R8" V 9230 4750 50  0000 C CNN
F 1 "DNP" V 9150 4750 50  0000 C CNN
F 2 "Resistor_SMD:R_0805_2012Metric" V 9080 4750 50  0001 C CNN
F 3 "~" H 9150 4750 50  0001 C CNN
	1    9150 4750
	1    0    0    -1  
$EndComp
$Comp
L kicad-library:AudioJack2_Ground J9
U 1 1 6184DC8B
P 8700 4500
F 0 "J9" H 8700 4850 50  0000 C CNN
F 1 "RX" H 8700 4750 50  0000 C CNN
F 2 "kicad-library:CUIDEVICES-SJ1-3523NG" H 8700 4500 50  0001 C CNN
F 3 "~" H 8700 4500 50  0001 C CNN
	1    8700 4500
	1    0    0    -1  
$EndComp
Wire Wire Line
	9150 5050 9150 4900
Wire Wire Line
	8700 4700 8700 5050
Wire Wire Line
	8700 5050 9150 5050
Wire Wire Line
	9150 4500 9150 4600
Wire Wire Line
	8900 4500 9150 4500
NoConn ~ 8900 4400
Wire Wire Line
	9150 4500 9150 4350
Wire Wire Line
	9150 4350 9450 4350
Connection ~ 9150 4500
Text Label 9450 4350 2    50   ~ 0
CUR2
Text Notes 8100 4800 0    50   ~ 0
BURDEN\nRESISTOR
Text Notes 6050 6550 0    50   ~ 0
1) Convert maximum RMS current to peak-current by multiplying by √2.\n     Primary peak-current = RMS current × √2 = 10 A × 1.414 = 14.14A\n2) Divide the peak-current by the number of turns in the CT to give the peak-current in the secondary coil.\n    Secondary peak-current = Primary peak-current / no. of turns = 14.14 A / 2000 = 0.00707A\n3) To maximise measurement resolution, the voltage across the burden resistor at peak-current should\n     be equal to one-half of the Arduino analog reference voltage. (AREF / 2)\n    Ideal burden resistance = floor((AREF/2) / Secondary peak-current) = floor(1.65 V / 0.00707 A) = 233 Ω
Connection ~ 9150 5050
Wire Wire Line
	9150 5050 9150 5200
Wire Wire Line
	9150 5500 9150 5650
Wire Wire Line
	8700 5050 7950 5050
Connection ~ 8700 5050
Connection ~ 7950 5050
Text Label 10200 5050 2    50   ~ 0
CURREF
Text Label 6850 1250 2    50   ~ 0
CURREF
Wire Wire Line
	6850 1250 6250 1250
Wire Wire Line
	6250 1350 6850 1350
Text Label 6850 1350 2    50   ~ 0
STMIO1
Text Label 3650 6800 0    50   ~ 0
STMIO1
Text Label 3650 6900 0    50   ~ 0
STMIO2
Text Label 6850 2150 2    50   ~ 0
STMIO2
Wire Wire Line
	6850 2150 6250 2150
Wire Wire Line
	10200 5050 9800 5050
$EndSCHEMATC
