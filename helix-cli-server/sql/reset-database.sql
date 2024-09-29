DROP DATABASE IF EXISTS helixc2;
CREATE DATABASE IF NOT EXISTS helixc2;

USE "helixc2";

CREATE TABLE IF NOT EXISTS targets (
    id INT,
    device_finger_print TEXT,
    ip TEXT,
    os TEXT,
    connection_time TIMESTAMP,
    disconnection_time TIMESTAMP
);

SELECT * FROM targets;
