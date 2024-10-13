# Helix C2
This project has been done for a university course called "The individual project course". According to the requirements list, this is supposed to be a C2 server which suites the needs of blue teamers and network admins controlling their remote devices and gathering a specific set of info, therefore the only available client option for now is a GUI client for Windows OS. In the future updates I will be adding payloads suitable for offensive operations, and perhaps change the current structure/functionalities completely.

<img src="https://github.com/user-attachments/assets/7e9b8275-d2ab-4034-87b2-7c909cda4235" alt="Helix" width="400"/>

### Current functionalities:

Functionalities of the current version of Helix include but are not limited to:
* Remote interactive CMD shell
* Gathers info such as Windows version, IP, HDD serial number and sends them to the server
* Backgroundable client GUI
* Installed application list
* Full interaction logs
* Server connection to mariadb for storing target info

### Server installation:

Install Mariadb: https://mariadb.org/
1) ``` sudo apt update && sudo apt upgrade ```
2) ``` sudo apt install nodejs ```
3) ``` sudo apt install npm ```
4) ``` git clone https://github.com/tr3sp4ss3rexe/Helix-C2.git ```
5) ``` cd Helix-C2 ```
6) ``` cd helix-cli-server ```
7) ``` npm i ```

### Server usage:
1) ``` cd sql ```
2) ``` mariadb < reset-database.sql ```
3) ``` sudo node helix.js ```
