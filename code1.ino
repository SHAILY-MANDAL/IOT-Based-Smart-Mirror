#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SoftwareSerial.h>

// OLED display width and height
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// Create an OLED display object
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Define SoftwareSerial for Bluetooth
SoftwareSerial BTSerial(10, 11); // TX on D10, RX on D11

// Days in each month
const int daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
const char *monthNames[] = {
    "January", "February", "March", "April", "May", "June",
    "July", "August", "September", "October", "November", "December"};

// Function to check if a year is a leap year
bool isLeapYear(int year) {
  return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

// Function to calculate the day of the week for the first day of a month
int calculateStartDay(int year, int month) {
  int day = 1;
  int y = year - (month < 3);
  int m = (month + 9) % 12 + 1;
  int k = y % 100;
  int j = y / 100;

  return (day + (13 * m + 1) / 5 + k + k / 4 + j / 4 - 2 * j + 7) % 7;
}

// Function to display the calendar on OLED
void displayCalendar(int year, int month) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);

  // Print month and year
  display.setCursor(0, 0);
  display.print(monthNames[month - 1]);
  display.print(" ");
  display.print(year);

  // Print weekdays header
  display.setCursor(0, 10);
  display.print("Su Mo Tu We Th Fr Sa");

  int days = daysInMonth[month - 1];
  if (month == 2 && isLeapYear(year)) {
    days = 29; // February in a leap year
  }

  int startDay = calculateStartDay(year, month);

  // Print the days
  int row = 1, col = startDay;
  for (int day = 1; day <= days; day++) {
    display.setCursor(col * 18, 20 + (row * 8)); // Adjusted row height
    if (day < 10) {
      display.print(" ");
    }
    display.print(day);

    col++;
    if (col > 6) {
      col = 0;
      row++;
    }
  }

  display.display();
}

void setup() {
  // Initialize software serial for Bluetooth
  BTSerial.begin(9600);

  // Initialize OLED display
  if (!display.begin(SSD1306_I2C_ADDRESS, 0x3C)) {
    while (1); // Loop forever if display initialization fails
  }
  display.clearDisplay();
  display.display();

  // Notify user via Bluetooth
  BTSerial.println("Enter month and year (MM YYYY):");
}

void loop() {
  if (BTSerial.available()) {
    String input = BTSerial.readStringUntil('\n');
    input.trim(); // Remove leading/trailing spaces

    int month, year;
    if (sscanf(input.c_str(), "%d %d", &month, &year) == 2) {
      if (month >= 1 && month <= 12 && year >= 1) {
        displayCalendar(year, month);
        BTSerial.println("Calendar displayed on OLED.");
      } else {
        BTSerial.println("Invalid input. Use MM YYYY format (e.g., 11 2024).");
      }
    } else {
      BTSerial.println("Invalid format. Use MM YYYY (e.g., 11 2024).");
    }
  }
}
