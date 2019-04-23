#ifndef FIRMWARE_H
#define FIRMWARE_H

/* AVR Headers */
#include <avr/io.h>
#include <avr/interrupt.h>

/* Firmware Headers */
#include <huart_controller.h>
#include <tuart_controller.h>
#include <tw_controller.h>
#include <nfc_controller.h>
#include <led_controller.h>
#include <port_controller.h>
#include <adc_controller.h>
#include <clock.h>

#define NUM_PORTS 6

#define PWR_MON_MASK   0xC0
#define PWR_MON_PGOOD  0x40
#define PWR_MON_CHG    0x80

#define XBEE_RST_PIN   0x20


class CFirmware {
public:
      
   static CFirmware& GetInstance() {
      static CFirmware cInstance;
      return cInstance;
   }

   int Execute();
      
private:

   /* private constructor */
   CFirmware() = default;

   bool bHasXbee;

   bool InitXbee();
   bool InitMPU6050();

   bool InitPCA9635();

   void DetectPorts();
   const char* GetPortString(CPortController::EPort ePort);

   void InteractiveMode();

   /* Test Routines */
   void TestAccelerometer();
   void TestPMIC();

   /* Reset */
   void Reset();

   struct SRxDetector : CNFCController::SRxFunctor {     
      virtual void operator()(const uint8_t* pun_data, uint8_t un_length) {
         if(LastRxTime) {
            *LastRxTime = CClock::GetInstance().GetMilliseconds();
         }
         Messages++;
      }
      uint32_t* LastRxTime = nullptr;
      uint16_t Messages = 0;
   };

   struct SFace {
      CPortController::EPort Port;
      bool Connected;
      SRxDetector RxAsTargetDetector;
      SRxDetector RxAsInitiatorDetector;
      uint32_t LastRxTime = 0;
      CNFCController NFC;
   };

   SFace m_psFaces[6] {
      { CPortController::EPort::NORTH },
      { CPortController::EPort::EAST },
      { CPortController::EPort::SOUTH },
      { CPortController::EPort::WEST },
      { CPortController::EPort::TOP },
      { CPortController::EPort::BOTTOM },
   };

   void Debug(const SFace& s_face);

};

#endif
