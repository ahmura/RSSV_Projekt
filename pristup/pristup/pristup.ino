#include <Keypad.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

constexpr uint8_t RST_PIN = 9;          // pin za RST rfid-a
constexpr uint8_t SS_PIN = 10;         // pin za SS rfid-a

int k=0;                      // brojac
byte pass[4];                 // varijabla opisuje unesen kod

bool pristupKod = false;        // varijabla za prepoznavanje pristupa preko koda
bool pristupKartice = false;    // varijabla za prepoznavanje pristupa preko kartice

bool unesenKod = false;         // varijabla za provjeru je li ispravan kod unesen 
bool unesenaKartica = false;    // varijabla za provjeru je li ispravna kartica prislonjena

const byte red = 4;    // Tipkovnica ima 4 reda
const byte stupac = 4; // i 4 stupca

MFRC522 mfrc522(SS_PIN, RST_PIN);       // inicijalizacija rfida sa SS i RST pinovima
 
  char tipke[red][stupac]  = {  // definiraj polozaj tipki na tipkovnici
   {'1','2','3','A'},
   {'4','5','6','B'},
   {'7','8','9','C'},
   {'*','0','#','D'}
};
 
  byte red_pinovi[red] = {8,7,6,5};             // pinovi spojeni na digital od 9 do 0
  byte stupac_pinovi[stupac] = {4,3,2,1};       // pinovi spojeni na digital od 9 do 0
   
  Keypad tipkovnica = Keypad(makeKeymap(tipke), red_pinovi, stupac_pinovi, red, stupac);      // inicijalizacije keypad-a
  LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);                              // inicijalizacija lcd ekrana za iic adapterom

void setup() {
  Serial.begin(9600);                 // zapocinjemo serijsku komunikaciju
  SPI.begin();                        // Init SPI bus
  mfrc522.PCD_Init();                 // Init MFRC522

  lcd.clear();                        // brisanje sadrzaja na lcd ekranu
  lcd.begin(16,2);                    // potrebno da bismo mogli koristiti LCD.
  lcd.backlight();                    // upali pozadinsko osvjetljenje
  lcd.print("   ZABRANJEN");     
  lcd.setCursor(0,1);                 // postavi kursor na pocetak drugog reda
  lcd.print("    PRISTUP");

}

void loop() {
  char utipkano = tipkovnica.getKey();  // registrira pritisnutu tipku
     
   if (utipkano) {               // kako bi izbjegli sumove, rezultat
    if(k <= 3) pass[k] = utipkano;          // unutar polja sprema vrijednosti
    k++;
    if(k > 4) k=0;
    if(utipkano == '#' ) {        // pritiskom # se potvrđuju unesene zadnje 4 znamenke
      unesenKod = true;
      k=0;
    }
   }

  if ((unesenKod==true) && (pass[0]=='1') && (pass[1]=='3') && (pass[2]=='3') && (pass[3]=='7')) {                  // provjera - je li unesen i ispravan kod
    pristupKod = true;
    unesenKod = false;
    /* RESET */
    pass[0]=='0';
    pass[1]=='0';
    pass[2]=='0';
    pass[3]=='0';
  }
  else{
    /* RESET */
    pass[0]=='0';
    pass[1]=='0';
    pass[2]=='0';
    pass[3]=='0';
    unesenKod = false;
  }

  if((mfrc522.uid.uidByte[0] == 68) && (mfrc522.uid.uidByte[1] == 114) && (mfrc522.uid.uidByte[2] == 68) && (mfrc522.uid.uidByte[3] == 3)){       // provjera - je li prislonjena ispravna kartica
    pristupKartice = true;
    mfrc522.uid.uidByte[0] = 0;
  }

  if(pristupKartice == true || pristupKod == true){
    lcd.clear();                        // brisanje sadrzaja na lcd ekranu
    lcd.begin(16,2);                    // potrebno da bismo mogli koristiti LCD.
    delay(500);                         // pricekaj 0.5s
    lcd.print("     LIJEP");     
    lcd.setCursor(0,1);                 // postavi kursor na pocetak drugog reda
    lcd.print("    POZDRAV!");
    delay(5000);                        // pricekaj 5s
    lcd.clear();                        // brisanje sadrzaja na lcd ekranu
    lcd.begin(16,2);                    // potrebno da bismo mogli koristiti LCD.
    lcd.print("ZAKLJUCAVAM...");
    delay(2500);                        // pricekaj 2.5s
    /* HARD RESET */
    pristupKartice = false;
    pristupKod = false;
    unesenKod = false;
    unesenKod = false;
    unesenaKartica = false;
    k = 0;
    lcd.clear();
    lcd.begin(16,2);
    lcd.print("   ZABRANJEN");
    lcd.setCursor(0,1);
    lcd.print("    PRISTUP");
  }


// Trazi nove kartice
 if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  // Odabire karticu
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return;
  }
}
