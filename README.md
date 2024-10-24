# Helix C2
This project has been done for a university course called "The individual project course" and is the very first version of Helix C2. According to the requirements list, this is supposed to be a C2 server which suites the needs of blue teamers and network admins controlling their remote devices and gathering a specific set of info, therefore the only available client option for now is a Windows based GUI client. In the future versions ALOT will change and I will be adding payloads suitable for offensive operations, and perhaps change the entire structure/functionalities completely.

<img src="https://github.com/tr3sp4ss3rexe/Helix-C2/blob/main/helix-nobg.png" alt="Helix" width="400"/>

### Current functionalities:

Functionalities of the current version of Helix include but are not limited to:
* Remote interactive CMD shell
* Gathers info such as Windows version, IP, HDD serial number and sends them to the server
* Backgroundable client GUI
* Installed application list
* Full interaction logs
* Server connection to mariadb for storing target info
* Port scanning

### Database installation:

Install and setup Mariadb: https://www.digitalocean.com/community/tutorials/how-to-install-mariadb-on-ubuntu-20-04

Modify ```my.cnf``` for easier database access: https://mariadb.com/kb/en/configuring-mariadb-with-option-files/

### Server installation:

1) ``` sudo apt update && sudo apt upgrade ```
2) ``` sudo apt install nodejs ```
3) ``` sudo apt install npm ```
4) ``` git clone https://github.com/tr3sp4ss3rexe/Helix-C2.git ```
5) ``` cd Helix-C2 ```
6) ``` cd helix-cli-server ```
7) ``` npm i ```
8) Go to ```config/db/helix.json``` and make the necessary changes to the DB config file
9) Go to ```sql``` directory and ``` mariadb < reset-database.sql ```

### Server usage:

On Linux:
``` bash RUNME.sh ```

Or: 
1) ``` cd sql ```
2) ``` mariadb < reset-database.sql ```
3) ``` cd .. ```
4) ``` sudo node helix.js ```

### Client setup and usage:

* Download a compiled and self-signed binary here: https://github.com/tr3sp4ss3rexe/Helix-C2/releases
* Or compile the code yourself in MS Visual Studio: https://github.com/tr3sp4ss3rexe/Helix-C2/tree/main/windows-gui-client

Note: If you are using the compiled version, do not forget to download the ICON file from windows-gui-client, so that you can see the app running in the tray when background function is used.

### License

* https://github.com/tr3sp4ss3rexe/Helix-C2?tab=GPL-3.0-1-ov-file
  
