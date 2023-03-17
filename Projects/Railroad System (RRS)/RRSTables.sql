CREATE TABLE TRAIN (
	train_number INTEGER NOT NULL,
	train_name varchar(20) NOT NULL,
	premium_fare INTEGER NOT NULL,
	general_fare INTEGER NOT NULL,
	source varchar(20) NOT NULL,
	destination varchar(20) NOT NULL,
	available_days varchar(52) NOT NULL,
	PRIMARY KEY (train_number, available_days)
);

CREATE TABLE PASSENGER (
	first_name VARCHAR(15) NOT NULL,
	last_name VARCHAR(15) NOT NULL,
	address VARCHAR(30),
	city VARCHAR(15),
	county VARCHAR(15),
	phone CHAR(12),
	SSN CHAR(9) NOT NULL,
	bdate DATE,
	PRIMARY KEY (SSN)
);

CREATE TABLE BOOKED (
	passenger_SSN CHAR(9) NOT NULL,
	train_number INTEGER NOT NULL,
	ticket_type VARCHAR(9) NOT NULL,
	reservation_status VARCHAR(9) NOT NULL,
	PRIMARY KEY(passenger_SSN, train_number),
	FOREIGN KEY (passenger_SSN) REFERENCES PASSENGER(SSN),
	FOREIGN KEY (train_number) REFERENCES TRAIN(train_number)
);

CREATE TABLE TRAIN_STATUS(
	train_date DATE,
    train_name VARCHAR(20) NOT NULL,
    premiumseats_available INTEGER NOT NULL,
    genseats_available INTEGER NOT NULL,
    premiumseats_occupied INTEGER NOT NULL,
    genseats_occupied INTEGER NOT NULL,
	PRIMARY KEY (train_name)
);
