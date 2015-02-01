#include <string>
#include <iostream>
#include <vector>
#include <fstream>
#include "ncl.h"
#include <vector>
#include <string>
#include <regex>


bool gNclInitialized = false;	//Global variable to maintain the state of the NCL
int gHandle = -1; //Global variable to maintain the current connected Nymi handle
std::vector<NclProvision> gProvisions; //Global vector for storing the list of provisioned Nymi
bool commflag = false;
int buffer[5];
void ECGpacket(int packet);
void callback(NclEvent event, void* userData){
	NclBool res;
	switch (event.type){
	case NCL_EVENT_INIT:
		if (event.init.success){
			gNclInitialized = true;
			std::cout << "log: init succeeded \n";
			//NclInfo info = nclInfo(); //Prints current initialization configuration
			//std::cout << info.string;
		}
		else exit(-1);
		break;
	case NCL_EVENT_ERROR:
		exit(-1);
		break;
	case NCL_EVENT_DISCOVERY:
		std::cout << "log: Nymi discovered\n";
		res = nclStopScan();	//Stops scanning to prevent discovering new Nymis
		if (res){
			std::cout << "Stopping Scan successful\n";
		}
		else{
			std::cout << "Stopping Scan failed\n";
		}

		gHandle = event.discovery.nymiHandle;
		res = nclAgree(gHandle); //Initiates the provisioning process with discovered Nymi
		if (res){
			std::cout << "Agree request successful\n";
		}
		else{
			std::cout << "Agree request failed\n";
		}
		break;
	case NCL_EVENT_FIND:
		std::cout << "log: Nymi found\n";
		res = nclStopScan(); //Stops scanning to prevent more find events
		if (res){
			std::cout << "Stopping Scan successful\n";
		}
		else{
			std::cout << "Stopping Scan failed\n";
		}

		gHandle = event.find.nymiHandle;
		res = nclValidate(gHandle); //Validates the found Nymi
		if (res){
			std::cout << "Validate request successful\n";
		}
		else{
			std::cout << "Validate request failed\n";
		}
		break;
	case NCL_EVENT_DISCONNECTION:
		std::cout << "log: disconnected\n";
		gHandle = -1; //Uninitialize the Nymi handle
		break;
	case NCL_EVENT_AGREEMENT:
		//Displays the LED pattern for user confirmation
		std::cout << "Is this:\n";
		for (unsigned i = 0; i<NCL_AGREEMENT_PATTERNS; ++i){
			for (unsigned j = 0; j<NCL_LEDS; ++j)
				std::cout << event.agreement.leds[i][j];
			std::cout << "\n";
		}
		std::cout << "the correct LED pattern (agree/reject)?\n";
		break;
	case NCL_EVENT_PROVISION:
		//Store the provision information in a vector. Ideally this information
		//is stored in persistent memory to be used later for future validations
		gProvisions.push_back(event.provision.provision);
		std::cout << "log: provisioned\n";
		break;
	case NCL_EVENT_VALIDATION:
		std::cout << "Nymi validated! Now trusted user requests can happen, such as request Symmetric Keys!\n";
		break;
	case NCL_EVENT_ECG:
		ECGpacket(event.ecg.samples[0]);
		break;
	default: break;
	}
}

void ECGpacket(int packet){
	static int chain = 0;
	static char buffer[500];
	static int i = 0;
	char emergency[] = { '.', '.', '.', '-', '-', '-', '.', '.', '.' };
	
	if (packet > 0) chain++;
	else if (chain > 7){
		std::cout << (chain > 30 ? "-" : ".");
		buffer[i] = (chain > 30 ? '-' : '.');
		i++;
		if (memcmp(&buffer[i-9], &emergency[0], 9 * sizeof(char)) == 0){
			std::cout << "Emergency!";
			std::fill(buffer, buffer + 500, 0);
			i = 0;
		}
		chain = 0;
	}
	else chain = 0;
}
int main(){
	std::cout << "Welcome to NymiCommunicator!\n";
	std::cout << "Enter \"provision\" if you want to start trusting a new Nymi.\n";
	std::cout << "Enter \"validate\" if you want to find trusted Nymis and validate the first one found.\n";
	std::cout << "Enter \"quit\" to quit.\n\n";

	if (!nclInit(callback, NULL, "NymiCommunicator", NCL_MODE_DEFAULT, stderr)) return -1;

	//Main loop for continuously polling user input
	while (true){
		std::string input;
		std::cin >> input; //retreives and stores user input

		//Ensures no commands are handled until NCL has completed initialization
		if (!gNclInitialized){
			std::cout << "error: NCL didn't finished initializing yet!\n";
			continue;
		}
		if (input == "provision"){
			NclBool res = nclStartDiscovery();
			if (res){
				std::cout << "Discovery started successfully\n";
				//std::cout << nclInfo().string;
			}
			else{
				std::cout << "Dsicovery failed to start\n";
			}
		}
		else if (input == "agree"){
			NclBool res = nclProvision(gHandle, NCL_FALSE);
			if (res){
				std::cout << "Provision request successful\n";

			}
			else{
				std::cout << "Provisioning failed\n";
			}
		}
		else if (input == "reject"){
			//Attempt to disconnect from currently connected Nymi
			if (!nclDisconnect(gHandle)){
				std::cout << "Disconnection Failed!\n";
			}
		}
		else if (input == "validate"){
			NclBool res = nclStartFinding(gProvisions.data(), gProvisions.size(), NCL_FALSE);
			if (res){
				std::cout << "Finding started successfully\n";
			}
			else{
				std::cout << "Finding failed to start\n";
			}
		}
		else if (input == "disconnect"){
			if (gHandle == -1){
				std::cout << "NEA Not connected to a Nymi. Cannot Disconnect\n";
				continue;
			}

			NclBool res = nclDisconnect(gHandle);
			if (res){
				std::cout << "Disconnection request successfull\n";
			}
			else{
				std::cout << "Disconnection failed\n";
			}
		}
		else if (input == "quit"){
			if (gHandle != -1){
				nclDisconnect(gHandle);
			}
			break;
		}
		else if (input == "communicator") {
			if (nclStartEcgStream(gHandle)) {
				std::cout << "Stream started. \n";
				commflag = true;
			}
			else std::cout << "Stream error.\n";

		}
		else if (input == "stopcommunicator") {
			commflag = false;
			std::cout << (nclStopEcgStream(gHandle) ? "Stream stopped \n" : "Stream could not be stopped. \n");
		}
		else{
			std::cout << "Unknown Command\n";
		}
	}

	nclFinish(); //closes the NCL
	return 0; //Quits program
}