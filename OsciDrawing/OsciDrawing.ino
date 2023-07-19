
int xPin = 10;
int yPin = 9;

#define arrayLen 20

double xValues[arrayLen] = {0, 0.1, 0.4, 0.6, 0.9, 1, 1  , 0.9  , 0.9  , 1  , 1, 0.9, 0.6, 0.4, 0.1, 0, 0  , 0.1  , 0.1  , 0  };
double yValues[arrayLen] = {0, 0  , 0.1  , .1  , 0  , 0, 0.1, 0.4, 0.6, 0.9, 1, 1  , 0.9  , 0.9  , 1  , 1, 0.9, 0.6, 0.4, 0.1};

double xValuesRot[arrayLen] = {0, 0.2, 0.4, 0.6, 0.8, 1, 1  , 1  , 1  , 1  , 1, 0.8, 0.6, 0.4, 0.2, 0, 0  , 0  , 0  , 0  };
double yValuesRot[arrayLen] = {0, 0  , 0  , 0  , 0  , 0, 0.2, 0.4, 0.6, 0.8, 1, 1  , 1  , 1  , 1  , 1, 0.8, 0.6, 0.4, 0.2};


int i = 0;

double shift = 0.0;

void setup() {
  // put your setup code here, to run once:

  for (int j = 0; j < arrayLen; j++) {
    xValues[j] -= 0.5;
    yValues[j] -= 0.5;
    xValues[j] *= 0.5;
    yValues[j] *= 0.5;
  }
  pinMode(xPin, OUTPUT);
  pinMode(yPin, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:

  for (int i = 0; i < arrayLen; i++) {
    xValuesRot[i] = rotateXCoord(xValues[i], yValues[i], 0.3);
    yValuesRot[i] = rotateYCoord(xValues[i], yValues[i], 0.3);
    xValues[i] = xValuesRot[i];
    yValues[i] = yValuesRot[i];
  }
  for (int i = 0; i < arrayLen; i++) {
    
    analogWrite(xPin, int((xValues[i]+0.5)*255)%255);
    analogWrite(yPin, int((yValues[i]+0.5)*255)%255);

    delay(5);
  }
  
}

double cyclicAdd(double x, double y, double thresh) {
  double result; 
  result = x + y;
  if(result > thresh) {
    result -= thresh;
  }
  return result;
}


double rotateXCoord(double x, double y, double theta) {
  return cos(theta)*x + sin(theta)*y;
}

double rotateYCoord(double x, double y, double theta) {
  return -sin(theta)*x + cos(theta)*y;
}
