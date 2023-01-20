#include <Common/Serial.h>
#include <Common/Log.h>
#include <Amd64/Amd64.h>

namespace Neon {
    namespace Serial {
        static void PrintCharacter(char Character, COMs COM) {
            while(!Amd64::inb(COM + 5 & (1 << 5))) asm("pause");
            Amd64::outb(COM, Character);
        }

        static char ReadCharacter(COMs COM) {
            while(!(Amd64::inb(COM + 5 & (1 << 0)))) asm("pause");
            return Amd64::inb(COM);
        }

        void InitializePort(COMs Port) {
            Amd64::outb(Port + 1, 0x00);
            Amd64::outb(Port + 3, 0x80);
            Amd64::outb(Port, 0x00);
            Amd64::outb(Port + 1, 0x00);
            Amd64::outb(Port + 3, 0x03);
            Amd64::outb(Port + 2, 0xC7);
            Amd64::outb(Port + 4, 0x0B);
        }

        void Initialize() {
            Log("[Serial] Initializing...");
            InitializePort(COM1);
            InitializePort(COM2);
            Log("[Serial] Done!");
        }
    }
}