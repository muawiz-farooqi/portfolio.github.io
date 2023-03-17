-- Queries 2-4

-- Query 2
-- Input a passenger’s last name and first name and retrieve all trains they are booked on.
SELECT DISTINCT t.train_name
FROM PASSENGER AS p, BOOKED AS b, TRAIN AS t
WHERE p.ssn = b.passenger_ssn AND p.first_name = 'Art' AND p.last_name = 'Venere' AND b.train_number = t.train_number AND b.reservation_status = 'Booked';

-- Query 3
-- Input the Day and list the passengers travelling on that day with confirmed tickets.
SELECT DISTINCT p.first_name, p.last_name
FROM PASSENGER AS p, TRAIN AS t, BOOKED AS b
WHERE t.available_days = 'Saturday' AND t.train_number = b.train_number AND b.reservation_status = 'Booked' AND b.passenger_ssn = p.ssn;

-- Query 4
-- User input the age of the passenger (50 to 60) and display the train information (Train Number, Train Name, Source and Destination)
-- and passenger information (Name, Address, Category, ticket status) of passengers who are between the ages of 50 to 60.
SELECT DISTINCT p.first_name, p.last_name, p.address, b.ticket_type AS category, b.reservation_status AS ticket_status, t.train_number, t.train_name, t.source, t.destination
FROM TRAIN AS t, PASSENGER AS p, BOOKED AS b
WHERE  ('2023-3-10') - bdate <= 60 AND ('2023-3-10') - bdate >= 50 AND p.ssn = b.passenger_ssn AND b.train_number = t.train_number
ORDER BY p.first_name, t.train_number;
