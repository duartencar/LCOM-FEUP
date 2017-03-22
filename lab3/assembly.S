#define STAT_REG    0x64
#define BIT(n) (0x01<<(n))
#define OUT_BUF 0x60

.global _lerTeclaAss
.text
xor %eax,%eax // Inicializar o registo a 0
_lerTeclaAss:
inb $STAT_REG
test $BIT(0), %al // 8 bits menos significativos do registo AND Bit(0), se 0, buffer está vazio
loopz _lerTeclaAss // Repete se não for 0
inb $OUT_BUF
ret

