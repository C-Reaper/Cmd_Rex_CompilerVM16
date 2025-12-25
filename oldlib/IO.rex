import "../lib/CStr.rex";
import "../lib/Math.rex";
import "../lib/System.rex";

define IO_FILE void*;

define O_RDONLY    0;        // Datei nur zum Lesen.
define O_WRONLY    1;        // Datei nur zum Schreiben.
define O_RDWR      2;        // Datei sowohl zum Lesen als auch zum Schreiben.
define O_CREAT     64;       // Erzeugt die Datei, falls sie nicht existiert. Wenn dieses Flag gesetzt ist, muss auch ein Berechtigungs-Mode angegeben werden (z. B. 0644 für Lese-/Schreibrechte).
define O_EXCL      128;      // Wird mit O_CREAT verwendet, um sicherzustellen, dass der Syscall fehlschlägt, wenn die Datei bereits existiert.
define O_TRUNC     512;      // Schneidet die Datei auf eine Länge von 0 ab, falls sie existiert und zum Schreiben oder zum Lesen/Schreiben geöffnet wurde.
define O_APPEND    1024;     // Setzt den Schreibzeiger an das Ende der Datei, sodass alle Schreibvorgänge ans Ende der Datei angehängt werden.
define O_NONBLOCK  2048;     // Öffnet die Datei im nicht-blockierenden Modus. Wird häufig für Spezialdateien wie Pipes und FIFO-Dateien verwendet.
define O_DSYNC     4096;     // Sorgt dafür, dass Schreiboperationen direkt auf die Festplatte geschrieben werden, sodass die Daten synchron sind, aber Metadaten (wie Änderungszeit) möglicherweise nicht.
define O_SYNC      1052672;  // Wie O_DSYNC, aber garantiert zusätzlich, dass alle Metadaten aktualisiert werden.
define O_NOFOLLOW  256;      // Verhindert das Folgen von symbolischen Links. Falls die Datei ein symbolischer Link ist, schlägt open mit einem Fehler fehl.
define O_CLOEXEC   524288;   // Setzt das FD_CLOEXEC-Flag, welches den Dateideskriptor automatisch schließt, wenn ein exec-Aufruf ausgeführt wird.
define O_DIRECTORY 65536;    // Öffnet die Datei nur, wenn sie ein Verzeichnis ist; anderenfalls schlägt der Syscall mit einem Fehler fehl.
define O_TMPFILE   4259840;  // Erzeugt eine temporäre Datei, die beim Schließen automatisch gelöscht wird, falls sie keinen Link im Dateisystem hat.

define SEEK_SET     0;        // Datei Anfang.
define SEEK_CUR     1;        // Datei relativ.
define SEEK_END     2;        // Datei Ende.

define STDIN        0;
define STDOUT       1;
define STDERR       2;

namespace io {
    void input(i8* cstr,u64 len){
        sys::read(0,cstr,len);
    }
    void print(i8* cstr){
        u64 len = cstr::len(cstr);
        sys::write(1,cstr,len);
    }
    void error(i8* cstr){
        u64 len = cstr::len(cstr);
        sys::write(2,cstr,len);
    }

    void put(u8 ch){
        i8 buff[2];
        *buff = ch;
        *(buff+1) = 0;
        sys::write(1,buff,2);
    }
    void uint(u64 n){
        i8 block[48];
        mem::set(block,0,48);
        math::uint::get(block,n);
        u64 len = cstr::len(block);
        sys::write(1,block,len);
    }
    void int(i64 n){
        i8 block[48];
        mem::set(block,0,48);
        math::int::get(block,n);
        u64 len = cstr::len(block);
        sys::write(1,block,len);
    }
    
    struct File[
        IO_FILE filedes
    ];

    impl File{
        pub File File::new(IO_FILE filedes){
            File file = File[ filedes ];
            return file;
        }
        pub File File::open(i8* filepath,i64 flags){
            IO_FILE filedes = sys::open(filepath,flags);
            File file = File[ filedes ];
            return file;
        }
        pub void File::close(File* self){
            sys::close(self->filedes);
            self->filedes = 0;
        }

        pub u64 File::size(File* self){
            sys::Stat filestate;
            sys::fstat(self->filedes,&filestate);
            return filestate.st_size;
        }
        pub void File::set_pos_start(File* self,u64 pos){
            sys::lseek(self->filedes,pos,SEEK_SET);
        }
        pub void File::set_pos_rel(File* self,u64 pos){
            sys::lseek(self->filedes,pos,SEEK_CUR);
        }
        pub void File::set_pos_end(File* self,u64 pos){
            sys::lseek(self->filedes,pos,SEEK_END);
        }

        pub i32 File::get_flags(File* self){
            return sys::fcntl(self->filedes,3,0);
        }
        pub i32 File::set_flags(File* self,i64 flags){
            return sys::fcntl(self->filedes,4,flags);
        }
        pub i32 File::add_flags(File* self,i64 flags){
            i64 oldflags = self->get_flags();
            return sys::fcntl(self->filedes,4,oldflags | flags);
        }
        pub i32 File::rm_flags(File* self,i64 flags){
            i64 oldflags = self->get_flags();
            return sys::fcntl(self->filedes,4,oldflags & ~flags);
        }

        pub i32 File::read(File* self,i8* cstr,i64 count){
            return sys::read(self->filedes,cstr,count);
        }
        pub void File::write(File* self,i8* cstr,i64 count){
            sys::write(self->filedes,cstr,count);
        }
    }
}