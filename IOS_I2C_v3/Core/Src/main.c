/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "usb_device.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "i2c_emulator.h"
#include "usbd_core.h"
#include "usbd_cdc_if.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

//Kodun daha anlaşılır olması için sayısal değerleri enum ile tanımladık. Bu değerler MFi dökümanına göre girildi.

enum ControlByte {
  SYN = 0x80,
  ACK = 0x40,
  EAK = 0x20,
  RST = 0x10,
  SLP = 0x08
};


enum AccessoryState {
  SUPPORT,
  LINK,
  CONTROL_SESSION,
  SESSION
};

enum AuthenticationState {
 RequestCertificate=0x00,
 RequestResponse=0x02,
 AuthFail=0x04,
 AuthSuccess=0x05
};

/*
 Bu struct yapısı debugging yaparken transmit edilen paketleri rahat görmek için oluşturuldu.
Certificate büyüklüğü 607 ile 609 arasında, oaket oluşturulunca sığması için 650 lik array kullandık.
Device tarafından gönderilen değerler de benzer bir şekilde kaydediliyor, ancak bu işlem usbd_cdc_if.c dosyasında
Receive fonksiyonu içinde yapılıyor.
*/
typedef  struct
{
    uint8_t packet[650];

}  DebugAccessoryStruct;



DebugAccessoryStruct AccessoryPackets[20];
uint8_t accessoryCount=0;
uint16_t cert_size_num;
/*
memcpy(&((AccessoryPackets[accessoryCount]).packet), iap2_link_packet_structure, packet_size);
accessoryCount++;

memcpy(&((DevicePackets[deviceCount]).packet), ReceiveBuffer, 64);
deviceCount++;
*/

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define true 1
#define false 0


#define SDA_ON (HAL_GPIO_WritePin(I2C_SDA_GPIO_Port, I2C_SDA_Pin, GPIO_PIN_SET))
#define SDA_OFF (HAL_GPIO_WritePin(I2C_SDA_GPIO_Port, I2C_SDA_Pin, GPIO_PIN_RESET))
#define SCL_ON (HAL_GPIO_WritePin(I2C_SCL_GPIO_Port, I2C_SCL_Pin, GPIO_PIN_SET))
#define SCL_OFF (HAL_GPIO_WritePin(I2C_SCL_GPIO_Port, I2C_SCL_Pin, GPIO_PIN_RESET))

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
uint8_t ReceiveBuffer[64]={0};
uint8_t * iap2_link_packet_structure;
uint8_t * iap2_session_payload;
uint8_t * parameter_payload;
uint8_t parsed_payload[32]={0};
uint8_t challengeResponse[64]={0};
volatile uint8_t packet_seq_num = 0x05;
volatile uint8_t packet_ack_num = 0x00;
volatile uint8_t device_seq_num=0x00;
//link seknronizasyonu için payload, dökümandaki default değerler girildi.
uint8_t link_sync_payload[]={0x01, 0x05, 0x10, 0x00, 0x07, 0xD0, 0x00, 0x16, 0x1E, 0x03,
                             0x0A, 0x00, 0x01, 0x0B, 0x02, 0x01};
/*
iap2 protokolünde fiziksel katman USB CDC dir. Bunun üzerinde link katmanı
yani link paketleri var. USB CDC Full Speed için maximum paket boyutu 64 byte, ancak link paketleri daha uzun
olması sorun değil sistem onları 64 olarak ayırarak low level da gönderiyor.

Link katmanında, link synchronization için senkronizasyon datası doğrudan payload olarak verilebilir.
 Link paketinin payload kısmı
*/
uint8_t iap2_packet[100]={0};
uint16_t packet_size;
uint16_t parameter_size;
uint16_t session_size;
volatile uint8_t accessoryState;
uint8_t check_flag=false;

uint16_t received_packet_length;
uint16_t received_payload_length;

uint8_t header_checksum;
uint8_t payload_checksum;

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
volatile uint8_t i2c_test1=false;
volatile uint8_t i2c_test2=false;
volatile uint8_t inside_flag=false;
volatile uint32_t delay;
uint8_t target_address=0x10;

uint8_t certificate_size[2]={0};
uint8_t * certificate;

uint8_t data_size;
uint8_t cert_buf[800]={0};


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
/* USER CODE BEGIN PFP */
uint8_t checksum_calculation(uint8_t *, uint16_t, uint16_t);
uint16_t create_packet(uint8_t, uint8_t *, uint16_t, uint8_t);
uint16_t create_parameter(uint16_t, uint8_t *, uint16_t);
uint16_t create_session_payload(uint16_t, uint8_t *, uint16_t);
void parse_control_session_payload(uint8_t *);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
uint8_t data[]= {0xFF,0x55,0x02, 0x00,0xEE,0x10};
volatile uint8_t configured_flag=false;
extern USBD_HandleTypeDef hUsbDeviceFS;

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */
  accessoryState=SUPPORT;

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
  I2C_Init(I2C_CP3_MODE);
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USB_DEVICE_Init();
  /* USER CODE BEGIN 2 */

  /*
   *
    I2C_Start();
  Tx(0x10);
  if(I2C_Read_SDA()==0){
  	  Tx(0x00);
  	  if(I2C_Read_SDA()==0){
    	     I2C_Stop();
    	     I2C_Start();
    	     Tx(0x11);
    	     if(I2C_Read_SDA()==0){
               challengeResponse[0]=Rx(1);
        	     I2C_Stop();
    	  	    }
  	    }
    }
for(int i=0; i<32; i++){
  		parsed_payload[i]=i*3;
  }


  I2C_Start();
  HAL_Delay(10);
  for(int i=0; i<32; i++){

	  Tx((target_address<<1)|0x00); //write to CP
	  Tx(0x21);
  	  Tx(parsed_payload[i]);

  }
  I2C_Stop();
  HAL_Delay(4000);
  i2c_test2=I2C_Receive(target_address, 0x12, challengeResponse, 64);



  */
  //şu anda i2c kısmı problemli olduğu için certificate önceden alıyoruz.
  //çünkü certificate için request gelince belirli süre içinde göndermek gerekiyor.
  //



  HAL_Delay(2000);
  i2c_test1=I2C_Receive(target_address,0x30,certificate_size,2);
  cert_size_num = (((uint16_t) (certificate_size[0])<<8)) + ((uint16_t) certificate_size[1]);
  certificate = (uint8_t*)calloc(cert_size_num, sizeof(uint8_t));
  HAL_Delay(4000);
  i2c_test2=I2C_Receive(target_address,0x31, certificate, 128);
  HAL_Delay(4000);
  i2c_test2=I2C_Receive(target_address,0x32, certificate+128, 128);
  HAL_Delay(4000);
  i2c_test2=I2C_Receive(target_address,0x33, certificate+256, 128);
  HAL_Delay(4000);
  i2c_test2=I2C_Receive(target_address,0x34, certificate+384, 128);
  HAL_Delay(4000);
  i2c_test2=I2C_Receive(target_address,0x35, certificate+512, 96);

  /*
  for(int i=0; i<round; i++){
	  HAL_Delay(4000);
	  i2c_test2=I2C_Receive(target_address,0x31+i, certificate+(128*round), 128);
  }
  i2c_test2=I2C_Receive(target_address, 0x31+round, certificate+(round)*128, cert_size_num - (round*128));
  */
  memcpy(cert_buf, certificate, cert_size_num);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	switch(accessoryState){
	case SUPPORT:
		if(ReceiveBuffer[0]==0){
			CDC_Transmit_FS(data, sizeof(data));
		    memcpy(&((AccessoryPackets[accessoryCount]).packet), data, sizeof(data));
		    accessoryCount++;
		    HAL_Delay(1000);
		}else{
		    accessoryState=LINK;
		         //----version---check---iAP2
			    for(int i=0; i<6; i++){
			    	 if(ReceiveBuffer[i]!=data[i]){
			    		 accessoryState=SUPPORT;
			    		   break;
			    		 }
			    	 }
		   memset (ReceiveBuffer, 0, 64);  // clear the buffer
		}
	    break;
	case LINK:
	  if(ReceiveBuffer[0]==0){
	  packet_size=create_packet(SYN, link_sync_payload, sizeof(link_sync_payload),0);
	  CDC_Transmit_FS(iap2_link_packet_structure, packet_size);
	  memcpy(&((AccessoryPackets[accessoryCount]).packet), iap2_link_packet_structure, packet_size);
	  accessoryCount++;
	  HAL_Delay(1000);
		}else{

			header_checksum=(checksum_calculation(ReceiveBuffer,0,8)==ReceiveBuffer[8]);
			received_packet_length = (((uint16_t) (ReceiveBuffer[2])<<8)) + ((uint16_t) ReceiveBuffer[3]);
			if(received_packet_length>9){
				received_payload_length=received_packet_length-10;
				payload_checksum=(checksum_calculation(ReceiveBuffer,9,received_payload_length)==ReceiveBuffer[received_packet_length-1]);
			}else{
				received_payload_length=0;
				payload_checksum=1;
			}

			//---Checksum-&-SYN+ACK--control---
		     if(header_checksum && payload_checksum && (ReceiveBuffer[4]==0xC0)){

		    //Transmission parameters negotiation check--ignore "maximum number of outstanding packets" (not negotiable)
		     for(int i=0; i<16; i++){
		    	                     // non-negotiable dışındaki parametrelerin uyuşması kontrolü, terslik varsa link durumuna geri dön
		    				    	 if((ReceiveBuffer[9+i]!=link_sync_payload[i]) && (i!=1 && i!=2 && i!=3)){
		    				    		 accessoryState=LINK;
		    				    		 memset (ReceiveBuffer, 0, 64);  // clear the buffer
		    				    		 break;
		    				    		 }
		    				    	 }
			 packet_ack_num=ReceiveBuffer[5];
		     uint8_t empty[]={0};
		     memset (ReceiveBuffer, 0, 64);  // clear the buffer
		     // transmission parametreleri uyuşuyor. control session durumuna geçilebilir. ACK yolla. State güncelle.
		     packet_size=create_packet(ACK, empty, 0, 0);
			 CDC_Transmit_FS(iap2_link_packet_structure, packet_size);
			 memcpy(&((AccessoryPackets[accessoryCount]).packet), iap2_link_packet_structure, packet_size);
			 accessoryCount++;// Debug için paketi kaydediyoruz
			 accessoryState=CONTROL_SESSION;

		     }else{

		    	//packet corrupted or different transmission parameters
		     }

		}

	  break;

	case CONTROL_SESSION:
		if(ReceiveBuffer[13] == 0xAA){
			if(ReceiveBuffer[14] == RequestCertificate){
				        packet_ack_num=ReceiveBuffer[5];
				        parameter_size = create_parameter(0x0000, certificate, cert_size_num);
				        session_size = create_session_payload(0xAA01, parameter_payload, parameter_size);
					    packet_size=create_packet(ACK, iap2_session_payload, session_size, 10);
					    memset (ReceiveBuffer, 0, 64);  // clear the buffer

					    CDC_Transmit_FS(iap2_link_packet_structure, packet_size);
					    memcpy(&((AccessoryPackets[accessoryCount]).packet), iap2_link_packet_structure, packet_size);
					    accessoryCount++; // Debug için paketi kaydediyoruz.

				}else if(ReceiveBuffer[14] == RequestResponse){ //check if challenge arrived
			    	packet_ack_num=ReceiveBuffer[5];
			    	parse_control_session_payload(ReceiveBuffer);
			    	// buradan sonrası sıkıntılı!!!! i2c ile challenge data gönderip response alma
			    	I2C_Start();
			    	Tx((target_address<<1)|0x00); //write to CP
			    	Tx(0x20);
			    	Tx(0x00);
			    	Tx(0x40);
			    	Tx(0x21);
			    	for(int i=0; i<32; i++){
				    	Tx(parsed_payload[i]);
			    	}
			    	I2C_Stop();
			  	    HAL_Delay(1000);
			    	i2c_test2=I2C_Receive(target_address, 0x12, challengeResponse, 64);
			        parameter_size = create_parameter(0x0000, challengeResponse, 64);
			    	session_size = create_session_payload(0xAA03, parameter_payload, parameter_size);
			    	packet_size=create_packet(ACK, iap2_session_payload, session_size, 10);
					CDC_Transmit_FS(iap2_link_packet_structure, packet_size);
				    memcpy(&((AccessoryPackets[accessoryCount]).packet), iap2_link_packet_structure, packet_size);
				    accessoryCount++;
			    } else if(ReceiveBuffer[14] == AuthSuccess){
		    		accessoryState=SESSION;
		    	} else{


		    	}




		}


	  break;

	case SESSION:


	  break;

	}

  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI48|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSI48State = RCC_HSI48_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSE;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, I2C_SDA_Pin|I2C_SCL_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : I2C_SDA_Pin I2C_SCL_Pin */
  GPIO_InitStruct.Pin = I2C_SDA_Pin|I2C_SCL_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */






//------------------I2C------BITBANGING---------CODE--------

void dly(){
	for (uint32_t i = 0; i < delay; i++){
	__ASM ("NOP");
	}
}

void I2C_Init(int mode) {

	SDA_ON;
	SCL_ON;
	delay = mode;

}

void I2C_Start(){
    SDA_ON;
    dly();
    SCL_ON;
    dly();
    SDA_OFF;
    dly();
    SCL_OFF;
    dly();
}

/*  i2c stop sequence */
void I2C_Stop(){
    SDA_OFF;
    dly();
    SCL_ON;
    dly();
    SDA_ON;
    dly();
}

uint8_t I2C_Read_SDA(void) {

	if (HAL_GPIO_ReadPin(I2C_SDA_GPIO_Port, I2C_SDA_Pin) == GPIO_PIN_SET) {
		return 1;
	} else {
		return 0;
	}
}

uint8_t I2C_Read_SCL(void) {

	if (HAL_GPIO_ReadPin(I2C_SCL_GPIO_Port, I2C_SCL_Pin) == GPIO_PIN_SET) {
		return 1;
	} else {
		return 0;
	}
}

/* Transmit 8 bit data to slave */
_Bool Tx(uint8_t dat){

    for(uint8_t i = 8; i; i--){
        (dat & 0x80) ? SDA_ON : SDA_OFF; //Mask for the eigth bit
        dat<<=1;  //Move
        dly();
        SCL_ON;
        dly();
        SCL_OFF;
        dly();
    }
    SDA_ON;
    SCL_ON;
    dly();
    _Bool ack = (I2C_Read_SDA()==0);    // Acknowledge bit
    SCL_OFF;
    return ack;
}

uint8_t Rx(_Bool ack){
    uint8_t dat = 0;
    SDA_ON;
    for( uint8_t i =0; i<8; i++){
        dat <<= 1;
        do{
            SCL_ON;
        }while(I2C_Read_SCL() == 0);  //clock stretching
        dly();
        if(I2C_Read_SDA()) dat |=1;
        dly();
        SCL_OFF;
    }
    ack ? SDA_OFF : SDA_ON;
    SCL_ON;
    dly();
    SCL_OFF;
    SDA_ON;
    return(dat);
}




_Bool I2C_Receive(uint8_t address, uint8_t reg, uint8_t *data, uint8_t size) {
	I2C_Start();
	if (Tx(address<<1)) {
		if(Tx(reg)){
			I2C_Stop();
			} else{
				return 0;
			}
		}else{
			return 0;
		}

	I2C_Start();
	if (Tx(address << 1 | 0x01)){ //start again, send address, read (LSB signifies R/W

			for (int j = 0; j < size; j++) {
				*data++ = Rx(1);
			}

			I2C_Stop();
			return true;
		}else{
			return false;
		}

}

//------iAP2 Checksum-------


uint8_t
checksum_calculation(uint8_t *buffer, uint16_t start, uint16_t length)
{
uint16_t i;
uint8_t sum = 0;
for (i = start; i < (start + length); i++) {
sum += buffer[i];
}
return (uint8_t)(0x100 - sum); /* 2's complement */
}

uint16_t create_packet(uint8_t ctrl_byte, uint8_t * payload, uint16_t payload_size, uint8_t session_identifier){
	uint16_t size;

	if(payload_size){
		size=10+payload_size;
	}else{
		size=9;
	}

	iap2_link_packet_structure = (uint8_t*)calloc(size, sizeof(uint8_t));
    if(iap2_link_packet_structure != NULL){ //Null pointer check

    	iap2_link_packet_structure[0]=0xFF;
    	iap2_link_packet_structure[1]=0x5A;
    	iap2_link_packet_structure[2]=(uint8_t) (size >> 8);
    	iap2_link_packet_structure[3]= (uint8_t) (size  & 0x00FF);
    	iap2_link_packet_structure[4]=ctrl_byte;
    	iap2_link_packet_structure[5]=packet_seq_num;
    	if(ctrl_byte & 0x40){ // check if ACK bit set
            iap2_link_packet_structure[6]=packet_ack_num;
    	}else{
            iap2_link_packet_structure[6]=0;
    	}
    	iap2_link_packet_structure[7]=session_identifier;
    	iap2_link_packet_structure[8]=checksum_calculation(iap2_link_packet_structure,0, 8);
        for(int i=0; i<payload_size; i++){
        	iap2_link_packet_structure[9+i]=payload[i];
        }
        if(payload_size!=0){
          iap2_link_packet_structure[size-1]=checksum_calculation(payload, 0, payload_size);
          packet_seq_num++;
        }

    }

    memcpy(&((AccessoryPackets[accessoryCount]).packet), iap2_link_packet_structure, size);
    accessoryCount++;

    return size;

}


uint16_t create_session_payload(uint16_t message_id, uint8_t * payload, uint16_t payload_size){
	    uint16_t size = payload_size + 6;

	    iap2_session_payload = (uint8_t*)calloc(size, sizeof(uint8_t));
	    if(parameter_payload != NULL){ //Null pointer check

	    	iap2_session_payload[0] = 0x40;
	    		iap2_session_payload[1] = 0x40;
	    		iap2_session_payload[2]=(uint8_t) (size >> 8);
	    		iap2_session_payload[3]= (uint8_t) (size  & 0x00FF);
	    		iap2_session_payload[4]=(uint8_t) (message_id >> 8);
	    		iap2_session_payload[5]= (uint8_t) (message_id  & 0x00FF);
	    		 for(int i=0; i<payload_size; i++){
	    			 iap2_session_payload[6+i]=payload[i];
	    				        }
	    }



		return size;

}

uint16_t create_parameter(uint16_t parameter_id, uint8_t * payload, uint16_t payload_size){
	    uint16_t size = payload_size + 4;

	    parameter_payload = (uint8_t*)calloc(size, sizeof(uint8_t));

	    if(parameter_payload != NULL){ //Null pointer check
	    	 parameter_payload[0]=(uint8_t) (size >> 8);
	    		    parameter_payload[1]= (uint8_t) (size  & 0x00FF);
	    		    parameter_payload[2]= (uint8_t) (parameter_id >> 8);
	    		    parameter_payload[3]= (uint8_t) (parameter_id  & 0x00FF);
	    			 for(int i=0; i<payload_size; i++){
	    				       parameter_payload[4+i]=payload[i];
    			        }

	    }else{


	    }


		 return size;
}


void parse_control_session_payload(uint8_t *packet){
	  uint16_t parsed_payload_size = packet[12]-10;
	  //parsed_payload = (uint8_t*)calloc(parsed_payload_size, sizeof(uint8_t));
	  memcpy(parsed_payload, packet+19, parsed_payload_size);

}



/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
