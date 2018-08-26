#include <avr/io.h>
#include <util/delay.h>

#include "rtc.h"

// Функция инициализация шины TWI
void I2CInit(void)
{
	// настройка TWI модуля
	TWBR = 0x48; // 100 мгц битрейт
	TWSR = (1 << TWPS1)|(1 << TWPS0); // Предделитель на 64
}

void I2CStart(void)
{
	// Передача условия СТАРТ
	TWCR = (1 << TWINT)|(1 << TWEN)|(1 << TWSTA);
	// Ожидание установки флага TWINT
	while(!(TWCR & (1 << TWINT)));
}

void I2CStop(void)
{
	TWCR = (1 << TWINT)|(1 << TWEN)|(1 << TWSTO); // Передача условия СТОП
	while(TWCR & (1 << TWSTO)); // Ожидание завершения передачи условия СТОП
}

// Функция записи данных по шине
uint8_t I2CWriteByte(uint8_t data)
{
	TWDR = data; // Загрузка данных в TWDR 
	TWCR = (1 << TWEN)|(1 << TWINT); // Сброс флага TWINT для начала передачи данных
	while(!(TWCR & (1 << TWINT))); // Ожидание завершения передачи
	// Проверка статуса
	if((TWSR & 0xF8) == 0x18 || (TWSR & 0xF8) == 0x28 || (TWSR & 0xF8) == 0x40)
	{
		// Если адрес DS1307, биты R/W и данные переданы
		// и получено подтверждение
		return 1;
	}
	else
		return 0; // ОШИБКА
}

// Функция чтения данных по шине
uint8_t I2CReadByte(uint8_t *data, uint8_t ack)
{
	if(ack) // Устанавливаем подтверждение
	{
		// Возвращаем подтверждение после приема
		TWCR |= (1 << TWEA);
	}
	else
	{
		// Возвращаем неподтверждение после приема
		// Ведомое устройство не получает больше данных
		// обычно используется для распознования последнего байта
		TWCR &= ~(1 << TWEA);
	}
	// Разрешение приема данных после сброса TWINT
	TWCR |= (1 << TWINT);
	while(!(TWCR & (1 << TWINT))); // Ожидание установки флага TWINT
	// Проверка статуса
	if((TWSR & 0xF8) == 0x58 || (TWSR & 0xF8) == 0x50)
	{
		// Прием данных и возвращение подтверждения
		//  или
		// Прием данных и возвращение неподтверждения
		*data = TWDR; // Читаем данные
		return 1;
	}
	else
		return 0; // Ошибка
}
// Функция чтения данных из DS1307
uint8_t DS1307Read(uint8_t address,uint8_t *data)
{
	uint8_t res; // Результат
	I2CStart(); // СТАРТ
	res = I2CWriteByte(0xD0); // адрес DS1307 + бит W
	if(!res)    return 0; // ОШИБКА
	// Передача адреса необходимого регистра
	res = I2CWriteByte(address);
	if(!res)    return 0; // ОШИБКА
	I2CStart(); // Повторный СТАРТ 
	res = I2CWriteByte(0xD1); // адрес DS1307 + бит R
	if(!res)    return 0; // ОШИБКА
	// Чтение данных с неподтверждением
	res = I2CReadByte(data,0);
	if(!res)    return 0; // ОШИБКА
	I2CStop(); // СТОП
	return 1;
}
// Функция записи данных в DS1307

uint8_t DS1307Write(uint8_t address,uint8_t data)
{
	uint8_t res; // Результат
	I2CStart(); // СТАРТ
	res = I2CWriteByte(0xD0); // адрес DS1307 + бит W
	if(!res)    return 0; // ОШИБКА
	// Передача адреса необходимого регистра
	res = I2CWriteByte(address);
	if(!res)    return 0; // ОШИБКА
	res = I2CWriteByte(data); // Запись данных
	if(!res)    return 0; // ОШИБКА
	I2CStop(); // СТОП
	return 1;
}

uint8_t ds1307_get_dec(uint8_t address)
{
	static uint8_t temp;
	DS1307Read(address ,&temp); // Reading time register
	return ((temp & 0xF0) >> 4)*10 + (temp & 0x0F); // FF to decimal system
}
