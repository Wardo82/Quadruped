#include <Servo.h>

float TODEG = 57.3; // Conversion from rad to deg

// ======================== Serial comunication variables ============================
String serialBuffer = "";  // Buffer for serial comunication
char received;             // Char received
String serialHeader;       // Header of the message (2 chars)
String serialBody;         // The body of the message recieved
// ===================================================================================


// ======================== Camera Servos ============================================
Servo S_camH;   // Horizontal axis servo
Servo S_camV;   // Vertical axis servo
int Ang_camH = 90;
int Ang_camV = 90;
// ===================================================================================


// ======================== Legs Servos & distances ===================================

// Distances of the leg
float coxa =  6.1;  // cm
float femur = 7.7;  // cm
float tibia = 13.7;  // cm

// Servos
Servo S_RF1;
Servo S_RF2;
Servo S_RF3;

Servo S_LF1;
Servo S_LF2;
Servo S_LF3;

Servo S_RB1;
Servo S_RB2;
Servo S_RB3;

Servo S_LB1;
Servo S_LB2;
Servo S_LB3;

// IK coords of each leg
double RFx, RFy, RFz;
double LFx, LFy, LFz;
double RBx, RBy, RBz;
double LBx, LBy, LBz;

// Offset angles of each servo
double offset1RF = 36;
double offset2RF = 100;
double offset3RF  = 43;

double offset1LF = 47;
double offset2LF = 46;
double offset3LF = 140;

double offset1LB = 51;
double offset2LB = 105;
double offset3LB = 39;

double offset1RB = 51;
double offset2RB = 46;
double offset3RB = 143;

// Safe servo angle limits
int r2 = 20;
int l2 = 135;


// ===================================================================================


// ======================== Setup ====================================================
void setup() {
  Serial.begin(115200);

  // Attach servos
  S_camH.attach(46);
  S_camV.attach(48);
  
  /*S_RF1.attach(2);
  S_RF2.attach(3);
  S_RF3.attach(4);
  S_LF1.attach(5);
  S_LF2.attach(6);
  S_LF3.attach(7);
  S_RB1.attach(8);
  S_RB2.attach(9);
  S_RB3.attach(10);
  S_LB1.attach(11);
  S_LB2.attach(12);
  S_LB3.attach(13);*/
  S_RF1.attach(22);
  S_RF2.attach(24);
  S_RF3.attach(26);
  S_LF1.attach(28);
  S_LF2.attach(30);
  S_LF3.attach(32);
  S_RB1.attach(34);
  S_RB2.attach(36);
  S_RB3.attach(38);
  S_LB1.attach(40);
  S_LB2.attach(42);
  S_LB3.attach(44);

  // Init coords
  RFx = 12;
  RFy = -3;
  RFz = 12;
  
  LFx = 12;
  LFy = -3;
  LFz = 12;

  RBx = 12;
  RBy = -3;
  RBz = 12;

  LBx = 12;
  LBy = -3;
  LBz = 12;

  movLegs(RFx,RFy,RFz, RBx,RBy,RBz, LFx,LFy,LFz, LBx,LBy,LBz, 10,10);
  wakeup(50,20);
}
// ===================================================================================



// ======================== Functions called via Serial ===================================

// Set camera servo positions
void updateCServos(){
  S_camH.write(Ang_camH);
  S_camV.write(Ang_camV);
}

void changePos(String var){
  
  int xx = var.substring(0,2).toInt();
  int yy = var.substring(2).toInt();

  movLegs(xx,yy,xx,      xx,yy,xx,    xx,yy,xx,        xx,yy,xx,     10,50);

  
}


// Move camera servos to the given angles
void controlCamera(String var){
  
  // Var = "xxxyyy" -> xxx = servo angle 1, yyy = servo angle 2

  int xxx = var.substring(0,3).toInt();
  int yyy = var.substring(3).toInt();

  if(xxx < 181 && xxx > -1 && yyy < 181 && yyy > -1){
    Ang_camH = xxx;
    Ang_camV = yyy;
  }
  updateCServos();
}


void test(String var){
  rotate2(1,30,30);
}

void st(String var){
  sit(30,30);
}

void wu(String var){
  wakeup(30,30);
}

void rr(String var){
  int xx = var.substring(0,2).toInt();
  rotate(xx,30,10);
}

void ww(String var){
  int xx = var.substring(0,2).toInt();
  
  if (xx > 0){
    walk1FW(10,10); 
  }else{
    walk1BW(10,10);
  }
}

void setLegsPos(String var){

  /*Serial.print(LBx);
  Serial.print(" ");
  Serial.print(LBy);
  Serial.print(" ");
  Serial.println(LBz);*/

  RFx = var.substring(0,3).toInt();
  RFy = var.substring(3,6).toInt();
  RFz = var.substring(6,9).toInt();

  LFx = var.substring(9,12).toInt();
  LFy = var.substring(12,15).toInt();
  LFz = var.substring(15,18).toInt();

  RBx = var.substring(18,21).toInt();
  RBy = var.substring(21,24).toInt();
  RBz = var.substring(24,27).toInt();

  LBx = var.substring(27,30).toInt();
  LBy = var.substring(30,33).toInt();
  LBz = var.substring(33,36).toInt();

  updateLServos();
  
  /*Serial.print(LBx);
  Serial.print(" ");
  Serial.print(LBy);
  Serial.print(" ");
  Serial.println(LBz);*/
  
}

// ===================================================================================


// ======================== Read serial communication ===============================

void readSerial(){

  while(Serial.available() > 0){
      
    received = Serial.read();
    
    if(received == ';'){              // End of the current message
  
      //Get message
      serialHeader = serialBuffer.substring(0, 2);
      serialBody = serialBuffer.substring(2);
  
      //Process header
      if(serialHeader == "cc"){   // Camera control
        controlCamera(serialBody);
      }else
      if(serialHeader == "t1"){   // Test
        test(serialBody);
      }else
      if(serialHeader == "rr"){
        rr(serialBody);
      }else
      if(serialHeader == "ik"){
        changePos(serialBody);
      }else
      if(serialHeader == "ww"){
        ww(serialBody);
      }else
      if(serialHeader == "ss"){
        setLegsPos(serialBody);
      }else
      if(serialHeader == "st"){
        st(serialBody);
      }else
      if(serialHeader == "wu"){
        wu(serialBody);
      }
  
      // Clear the buffer
      serialBuffer = "";

      // Send Arduino Ready signal
      Serial.println("1");
      
    }else{          // Not the end of current message
      serialBuffer += received; //Add the received char to the buffer
    }
  }
}
// ===================================================================================



// ======================== Movements routines =======================================


void sit(int div, int time){
  movLegs(12,-3,12,      12,-3,12,     12,-3,12,       12,-3,12,     div,time);
}

void wakeup(int div, int time){
  movLegs(12,11,12,      12,11,12,     12,11,12,       12,11,12,     div,time);
}


void walk1FW(int div, int time){
  int zmax = 14;
  int zmin = 1;
  int x = 12;
  int up = 1;
  int down = 11;
  int down2 = 8;
  
  float a[11];
  for (int i=0; i<11; i++){
    a[i] = zmin + (float(zmax-zmin)/10)*i;
    Serial.print(i);
    Serial.print(" - ");
    Serial.println(a[i]);
  }
  //{10,-2,-1,-1,1,3,5,7,9,11,13,15};
  //{1, 2.3, 3.6, 4.9, 6.2, 7.5, 8.8, 10.1, 11.4, 12.7, 14};

//movLegs(RFx,RFy,RFz,      RBx,RBy,RBz,    LFx,LFy,LFz,        LBx,LBy,LBz,     div,time);
  movLegs(x,down,a[3],      x,up,a[10],     x,down2,a[9],       x,down,a[4],     div,time);
  movLegs(x,down,a[2],      x,up,a[0],      x,down2,a[8],        x,down,a[5],     div,time);
  movLegs(x,down,a[1],      x,down,a[0],    x,down,a[7],        x,down,a[6],     div,time);
  
  movLegs(x,up,a[0],        x,down,a[1],    x,down,a[6],        x,down2,a[7],     div,time);
  movLegs(x,up,a[10],       x,down,a[2],    x,down,a[5],        x,down2,a[8],     div,time);
  movLegs(x,down,a[10],     x,down,a[3],    x,down,a[4],        x,down,a[9],     div,time);
  
  movLegs(x,down2,a[9],     x,down,a[4],    x,down,a[3],        x,up,a[10],      div,time);
  movLegs(x,down2,a[8],     x,down,a[5],    x,down,a[2],        x,up,a[0],       div,time);
  movLegs(x,down,a[7],      x,down,a[6],    x,down,a[1],        15,down,a[0],    div,time);
  
  movLegs(x,down,a[6],      x,down2,a[7],   x,up,a[0],          x,down,a[1],     div,time);
  movLegs(x,down,a[5],      x,down2,a[8],   x,up,a[10],         x,down,a[2],     div,time);
  movLegs(x,down,a[4],      x,down,a[9],    x,down,a[10],       x,down,a[3],     div,time);
}

void walk1BW(int div, int time){
  int zmax = 14;
  int zmin = 1;
  int x = 12;
  int up = 1;
  int down = 11;
  int down2 = 8;
  
  float a[11];
  for (int i=10; i>-1; i--){
    a[i] = zmin + (float(zmax-zmin)/10)*(10-i);
    Serial.print(i);
    Serial.print(" - ");
    Serial.println(a[i]);
  }

//movLegs(RFx,RFy,RFz,      RBx,RBy,RBz,    LFx,LFy,LFz,        LBx,LBy,LBz,     div,time);
  movLegs(x,down,a[3],      x,up,a[10],     x,down2,a[9],       x,down,a[4],     div,time);
  movLegs(x,down,a[2],      x,up,a[0],      x,down2,a[8],        x,down,a[5],     div,time);
  movLegs(x,down,a[1],      x,down,a[0],    x,down,a[7],        x,down,a[6],     div,time);
  
  movLegs(x,up,a[0],        x,down,a[1],    x,down,a[6],        x,down2,a[7],     div,time);
  movLegs(x,up,a[10],       x,down,a[2],    x,down,a[5],        x,down2,a[8],     div,time);
  movLegs(x,down,a[10],     x,down,a[3],    x,down,a[4],        x,down,a[9],     div,time);
  
  movLegs(x,down2,a[9],     x,down,a[4],    x,down,a[3],        x,up,a[10],      div,time);
  movLegs(x,down2,a[8],     x,down,a[5],    x,down,a[2],        x,up,a[0],       div,time);
  movLegs(x,down,a[7],      x,down,a[6],    x,down,a[1],        15,down,a[0],    div,time);
  
  movLegs(x,down,a[6],      x,down2,a[7],   x,up,a[0],          x,down,a[1],     div,time);
  movLegs(x,down,a[5],      x,down2,a[8],   x,up,a[10],         x,down,a[2],     div,time);
  movLegs(x,down,a[4],      x,down,a[9],    x,down,a[10],       x,down,a[3],     div,time);
}

void walk2(int div, int time){
  int zmax = 14;
  int zmed = 11;
  int zmin = 8;
  int x = 12;
  int up = 1;
  int down = 11;
  int down2 = 8;

//movLegs(RFx,RFy,RFz,      RBx,RBy,RBz,    LFx,LFy,LFz,        LBx,LBy,LBz,     div,time);
  movLegs(x,up,zmed,        x,down,zmed,     x,down,zmed,     x,up,zmed,       div,time);
  movLegs(x,down,zmax,      x,down,zmax,     x,down,zmin,     x,down,zmin,     div,time);
  
  movLegs(x,down,zmed,      x,up,zmed,       x,up,zmed,      x,down,zmed,      div,time);
  movLegs(x,down,zmin,      x,down,zmin,     x,down,zmax,     x,down,zmax,     div,time);


}

void rotate(int dir, int div, int time){
  // dir =1 -> right  ,,  dir=-1 ->left
  
  int x0 = 9;
  int z0 = 9;

  int rotation = 40; // º
  int down = 10;
  int up = 1;
  
  movLegs(x0,down,z0,      x0,down,z0,    x0,down,z0,        x0,down,z0,     div,time);
  movLegs(x0,up,z0,      x0,down,z0,    x0,down,z0,        x0,up,z0,     div,time);
  rot(-rotation*dir,0,0,-rotation*dir,   div,time);
  movLegs(RFx,down,RFz,      RBx,down,RBz,    LFx,down,LFz,        LBx,down,LBz,     div,time);
    rot(rotation*dir,-rotation*dir,-rotation*dir,rotation*dir,   div,time);
  movLegs(RFx,down,RFz,      RBx,up,RBz,    LFx,up,LFz,        LBx,down,LBz,     div,time);
  rot(0,rotation*dir,rotation*dir,0,   div,time);
  movLegs(RFx,down,RFz,      RBx,down,RBz,    LFx,down,LFz,        LBx,down,LBz,     div,time);
  
  
  updateLServos();
  
  
}


void rotate2(int dir, int div, int time){
  // dir =1 -> right  ,,  dir=-1 ->left
  
  int x0 = 12;
  int z0 = 12;

  int rotation = 25; // º
  int down = 11;
  int up = 1;
  
  movLegs(RFx,up,RFz,      RBx,RBy,RBz,    LFx,LFy,LFz,        LBx,LBy,LBz,     div,time);
  rot(rotation*dir,0,0,0,   div,time);
  movLegs(RFx,down,RFz,      RBx,RBy,RBz,    LFx,LFy,LFz,        LBx,LBy,LBz,     div,time);

  movLegs(RFx,RFy,RFz,      RBx,up,RBz,    LFx,LFy,LFz,        LBx,LBy,LBz,     div,time);
  rot(0,-rotation*dir,0,0,   div,time);
  movLegs(RFx,RFy,RFz,      RBx,down,RBz,    LFx,LFy,LFz,        LBx,LBy,LBz,     div,time);

  movLegs(RFx,RFy,RFz,      RBx,RBy,RBz,    LFx,LFy,LFz,        LBx,up,LBz,     div,time);
  rot(0,0,0,rotation*dir,  div,time);
  movLegs(RFx,RFy,RFz,      RBx,RBy,RBz,    LFx,LFy,LFz,        LBx,down,LBz,     div,time);

  movLegs(RFx,RFy,RFz,      RBx,RBy,RBz,    LFx,up,LFz,        LBx,LBy,LBz,     div,time);
  rot(0,0,-rotation*dir,0,   div,time);
  movLegs(RFx,RFy,RFz,      RBx,RBy,RBz,    LFx,down,LFz,        LBx,LBy,LBz,     div,time);

  rot(-rotation*dir,rotation*dir,rotation*dir,-rotation*dir,   div,time);

  
}



void testIK1(int div, int time){
  movLegs(25,2,25,     RBx,RBy,RBz,     LFx,LFy,LFz,        LBx,LBy,LBz,    div,time);
  Serial.print("1: ");
  Serial.print(S_RF1.read());
  Serial.print(" 2: ");
  Serial.print(S_RF2.read());
  Serial.print(" 3: ");
  Serial.println(S_RF3.read());
  
  movLegs(18,2,18,      RBx,RBy,RBz,      LFx,LFy,LFz,        LBx,LBy,LBz,    div,time);
  Serial.print("1: ");
  Serial.print(S_RF1.read());
  Serial.print(" 2: ");
  Serial.print(S_RF2.read());
  Serial.print(" 3: ");
  Serial.println(S_RF3.read());

  movLegs(18,7,18,      RBx,RBy,RBz,      LFx,LFy,LFz,        LBx,LBy,LBz,    div,time);
  Serial.print("1: ");
  Serial.print(S_RF1.read());
  Serial.print(" 2: ");
  Serial.print(S_RF2.read());
  Serial.print(" 3: ");
  Serial.println(S_RF3.read());

  movLegs(18,2,18,      RBx,RBy,RBz,      LFx,LFy,LFz,        LBx,LBy,LBz,    div,time);
  Serial.print("1: ");
  Serial.print(S_RF1.read());
  Serial.print(" 2: ");
  Serial.print(S_RF2.read());
  Serial.print(" 3: ");
  Serial.println(S_RF3.read());

  movLegs(25,2,25,      RBx,RBy,RBz,      LFx,LFy,LFz,        LBx,LBy,LBz,    div,time);
  Serial.print("1: ");
  Serial.print(S_RF1.read());
  Serial.print(" 2: ");
  Serial.print(S_RF2.read());
  Serial.print(" 3: ");
  Serial.println(S_RF3.read());
  
  movLegs(25,7,25,      RBx,RBy,RBz,      LFx,LFy,LFz,        LBx,LBy,LBz,    div,time);
  Serial.print("1: ");
  Serial.print(S_RF1.read());
  Serial.print(" 2: ");
  Serial.print(S_RF2.read());
  Serial.print(" 3: ");
  Serial.println(S_RF3.read());

  movLegs(18,7,18,      RBx,RBy,RBz,      LFx,LFy,LFz,        LBx,LBy,LBz,    div,time);
  Serial.print("1: ");
  Serial.print(S_RF1.read());
  Serial.print(" 2: ");
  Serial.print(S_RF2.read());
  Serial.print(" 3: ");
  Serial.println(S_RF3.read());

  movLegs(18,2,18,      RBx,RBy,RBz,      LFx,LFy,LFz,        LBx,LBy,LBz,    div,time);
  Serial.print("1: ");
  Serial.print(S_RF1.read());
  Serial.print(" 2: ");
  Serial.print(S_RF2.read());
  Serial.print(" 3: ");
  Serial.println(S_RF3.read());
}


void testIK2(int div, int time){
  Serial.println("Centro");
  movLegs(13,8,13,     13,8,13,     13,8,13,        13,8,13,    div,time);
  Serial.print("1: ");
  Serial.print(S_RF1.read());
  Serial.print(" 2: ");
  Serial.print(S_RF2.read());
  Serial.print(" 3: ");
  Serial.println(S_RF3.read());
  delay(300);
  Serial.println("Derecha");
  movLegs(18,8,0,     18,8,0,     18,8,0,        18,8,0,    div,time);
    Serial.print("1: ");
  Serial.print(S_RF1.read());
  Serial.print(" 2: ");
  Serial.print(S_RF2.read());
  Serial.print(" 3: ");
  Serial.println(S_RF3.read());
  delay(300);/*
  Serial.println("Centro");
  movLegs(13,8,13,    RBx,RBy,RBz,     LFx,LFy,LFz,        LBx,LBy,LBz,    div,time);
    delay(300);
  Serial.println("Izquierda");
  movLegs(0,8,18,     RBx,RBy,RBz,     LFx,LFy,LFz,        LBx,LBy,LBz,    div,time);
    Serial.print("1: ");
  Serial.print(S_RF1.read());
  Serial.print(" 2: ");
  Serial.print(S_RF2.read());
  Serial.print(" 3: ");
  Serial.println(S_RF3.read());
  delay(300);*/
}

void testIK3(int div, int time){
  int x = 17;
  Serial.println("Abajo");
  movLegs(x,5,x,     x,5,x,     x,5,x,        x,5,x,    div,time);
  delay(500);
  Serial.println("Arriba");
  movLegs(14,11,x,     x,11,x,     x,11,x,        x,11,x,    div,time);
  delay(500);
}


void testIK4(int div, int time){
  double xx=17;//14;
  double zz=17;//14;
  double yy=13.86;//13.15;
  movLegs(xx,yy,zz,     xx,yy,zz,     xx,yy,zz,        xx,yy,zz,    div,time);
}


void testIK5(int div, int time){
  movLegs(15,3,-2,        RBx,RBy,RBz,    LFx,LFy,LFz,        LBx,LBy,LBz,   div,time);
  Serial.println(S_RF2.read());
}
// ===================================================================================


// ======================== IK Movements functions =============================================

// Linearly move all legs to the given coordinates
void movLegs(double t_RFx, double t_RFy, double t_RFz, double t_RBx, double t_RBy, double t_RBz, double t_LFx, double t_LFy, double t_LFz, double t_LBx, double t_LBy, double t_LBz, int div, int time){
  
  // div -> divisions (number of movements to reach the new coordinates)
  // time-> time between each sub-movement

  double subxRF = (t_RFx - RFx)/div;  // Increment to be added to the current position to reach the destination in "div" movements
  double subyRF = (t_RFy - RFy)/div;
  double subzRF = (t_RFz - RFz)/div;
  
  double subxRB = (t_RBx - RBx)/div;
  double subyRB = (t_RBy - RBy)/div;
  double subzRB = (t_RBz - RBz)/div;
  double subxLF = (t_LFx - LFx)/div;
  double subyLF = (t_LFy - LFy)/div;
  double subzLF = (t_LFz - LFz)/div;
  double subxLB = (t_LBx - LBx)/div;
  double subyLB = (t_LBy - LBy)/div;
  double subzLB = (t_LBz - LBz)/div;

  // Do the "div" movements
  for(int i = 0; i < div; i++){
    RFx += subxRF;
    RFy += subyRF;
    RFz += subzRF;
    RBx += subxRB;
    RBy += subyRB;
    RBz += subzRB;
    LFx += subxLF;
    LFy += subyLF;
    LFz += subzLF;
    LBx += subxLB;
    LBy += subyLB;
    LBz += subzLB;

    updateLServos();
    
    delay(time);
  } 
}


// Rotate leegs x degrees
void rot(double t_RFr, double t_RBr, double t_LFr, double t_LBr, int div, int time){
  
  // div -> divisions (number of lines for the circle)
  // time-> time between each sub-movement
  // t_XXr -> angle to rotate in degrees

  float rRF = sqrt(RFx*RFx + RFz*RFz);  // Radius
  float rLF = sqrt(LFx*LFx + LFz*LFz);
  float rLB = sqrt(LBx*LBx + LBz*LBz);
  float rRB = sqrt(RBx*RBx + RBz*RBz);

  float aRF = atan2(RFz,RFx);    // Current angle
  float aLF = atan2(LFz,LFx);
  float aRB = atan2(RBz,RBx);
  float aLB = atan2(LBz,LBx);

  double subRF = (t_RFr/div)/TODEG;   // Radius to incremet in rad
  double subRB = (t_RBr/div)/TODEG;
  double subLF = (t_LFr/div)/TODEG;
  double subLB = (t_LBr/div)/TODEG;

  for(int i = 0; i < div; i++){
    
    aRF += subRF;
    aRB += subRB;
    aLF += subLF;
    aLB += subLB;

    RFx = cos(aRF)*rRF;
    RFz = sin(aRF)*rRF;

    LFx = cos(aLF)*rLF;
    LFz = sin(aLF)*rLF;

    RBx = cos(aRB)*rRB;
    RBz = sin(aRB)*rRB;

    LBx = cos(aLB)*rLB;
    LBz = sin(aLB)*rLB;
  
    updateLServos();
    
    delay(time);
  } 
}




// Linearly move the body
void moveAll(double x, double y, double z, int div, int time){
  
  double subx = x/div;
  double suby = y/div;
  double subz = z/div;

  for(int i = 0; i < div; i++){
    RFx += subx;
    RFy += suby;
    RFz -= subz;

    LFx -= subx;
    LFy += suby;
    LFz -= subz;

    RBx += subx;
    RBy += suby;
    RBz += subz;

    LBx -= subx;
    LBy += suby;
    LBz += subz;

    updateLServos();
    
    delay(time);
  }
}
// ===================================================================================

// ======================== IK functions =============================================
void updateLServos(){
    // Set the servos to their coords with IK functions
    S_RF1.write(180 - (offset1RF + IKang1(RFx,RFy,RFz)));
    S_RF2.write(max(r2, 90 + offset2RF - IKang2(RFx,RFy,RFz)));
    S_RF3.write((180+offset3RF) - IKang3(RFx,RFy,RFz));

    S_LF1.write(IKang1(LFx,LFy,LFz) + offset1LF);
    S_LF2.write(min(l2, IKang2(LFx,LFy,LFz) + offset2LF - 90));
    S_LF3.write(offset3LF + (IKang3(LFx,LFy,LFz)-180));
    
    S_RB1.write(IKang1(RBx,RBy,RBz) + offset1RB);
    S_RB2.write(min(l2, IKang2(RBx,RBy,RBz) + offset2RB - 90));
    S_RB3.write(offset3RB + (IKang3(RBx,RBy,RBz) - 180));
    
    S_LB1.write(180 - (offset1LB + IKang1(LBx,LBy,LBz)));
    S_LB2.write(max(r2, 90 + offset2LB - IKang2(LBx,LBy,LBz)));
    S_LB3.write((180+offset3LB) - IKang3(LBx,LBy,LBz));

    Serial.print(S_LB1.read());
    Serial.print(" ");
    Serial.print(S_LB2.read());
    Serial.print(" ");
    Serial.println(S_LB3.read());
}

// Angle for servo 1
int IKang1(double x, double y, double z){
  double gamma = atan2(z, x);
  //Serial.println(gamma*TODEG);
  return (gamma * TODEG);
}

// Angle for servo 2
int IKang2(double x, double y, double z){
  /*double g = atan2(z, x);
  double xz = (x-coxa)*cos(g) + (z-coxa)*sin(g);
  double hip = sqrt( pow(y,2) + pow((xz),2));
  
  double alpha1 = acos(y/hip);
  double alpha2 = acos( (pow(femur, 2) + pow(hip,2) - pow(tibia,2))/(2*femur*hip));
  double alpha = alpha1 + alpha2;
  //Serial.println(alpha*TODEG);*/

  double h = sqrt((x*x) + (z*z)) - coxa;
  double a0 = atan2(h,y);
  double a1 = acos( ((femur*femur) + (h*h) + (y*y) - (tibia*tibia)) / (2*femur*sqrt((h*h) + (y*y))));

  double alpha = a0+a1;
    
  return (alpha * TODEG);
}

// Angle for servo 3
int IKang3(double x, double y, double z){
  /*double g = atan2(z, x);
  double xz = (x-coxa)*cos(g) + (z-coxa)*sin(g);
  double hip = sqrt( pow(y,2) + pow((xz),2));
  
  double beta = acos(( pow(femur,2) + pow(tibia,2) - pow(hip,2) )/(2*tibia*femur));
  //Serial.println(beta*TODEG);*/

  double h = sqrt((x*x) + (z*z)) - coxa;

  double beta = acos(( (tibia*tibia) + (femur*femur) - (y*y) - (h*h) ) / (2*femur*tibia));
  
  return (beta * TODEG);
}
// ===================================================================================




void loop() {

  readSerial();
  /*Serial.print("1: ");
  Serial.print(S_RF1.read());
  Serial.print(" 2: ");
  Serial.print(S_RF2.read());
  Serial.print(" 3: ");
  Serial.println(S_RF3.read());*/
 //walk1FW(10,10);
 //rotate(1, 30,60); 
  //walk2(30,10);
  //testIK3(50,30);

  
  delay(10);

}
