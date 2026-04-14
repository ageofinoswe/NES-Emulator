#include "Cpu.h"
#include "json.hpp"

int main()
{
    // opcode test file
    std::ifstream testFile("../tests/SST/ADC/71.json");
    nlohmann::json tests = nlohmann::json::parse(testFile);
    
    // opcode test results
    std::ofstream testResults("../tests/testResults.txt");

    // number of tests passed and failed
    bool pass = true;
    int passTest = 0;
    int failTest = 0;

    testResults << "****************************\n";
    testResults << "Test Results for OpCode 0x6d\n";   
    testResults << "****************************\n";

    int testNum = 1;
    std::vector<int> initialMemory = {};
    std::vector<int> finalMemory = {};
    // iterate through all tests for the opcode
    for (auto it = tests.begin(); it != tests.end(); ++it)
    {
/*         std::cout << "Test Number: " << testNum << "\n";
 */
        // instantiate new bus and cpu for testing
        Bus bus(false); 
        Cpu cpu(bus);

        // extract opcode tests from file
        nlohmann::json initialCpuState = (*it)["initial"];
        nlohmann::json initialMemState = initialCpuState["ram"];

        // set the cpu state with corresponding opcode test register values
        cpu.setTestCpuState("PC", initialCpuState["pc"].get<int>());
        cpu.setTestCpuState("S", initialCpuState["s"].get<int>());
        cpu.setTestCpuState("A", initialCpuState["a"].get<int>());
        cpu.setTestCpuState("X", initialCpuState["x"].get<int>());
        cpu.setTestCpuState("Y", initialCpuState["y"].get<int>());
        cpu.setTestCpuState("P", initialCpuState["p"].get<int>());
        uint16_t initialTestPC = cpu.getRegisterValue("PC");
        uint16_t initialTestS = cpu.getRegisterValue("S");
        uint16_t initialTestA = cpu.getRegisterValue("A");
        uint16_t initialTestX = cpu.getRegisterValue("X");
        uint16_t initialTestY = cpu.getRegisterValue("Y");
        uint16_t initialTestP = cpu.getRegisterValue("P");

        // set the memory with opcode test values
        for (auto itMem = initialMemState.begin(); itMem != initialMemState.end(); ++itMem)
        {
            int address = (*itMem)[0].get<int>();
            int value = (*itMem)[1].get<int>();

            initialMemory.push_back(address);
            initialMemory.push_back(value);

            cpu.writeTestMem(address, value);
            initialMemory.push_back(cpu.readTestMem(address));
        }

        // auto run all cycles to get final state of cpu
        cpu.autoRunCycles();

        // extract opcode test final state
        nlohmann::json finalCpuState = (*it)["final"];
        nlohmann::json finalMemState = finalCpuState["ram"];

        // compare cpu state with opcode test final state
        pass = pass && (finalCpuState["pc"].get<int>() == cpu.getRegisterValue("PC"));
        pass = pass && (finalCpuState["s"].get<int>() == cpu.getRegisterValue("S"));
        pass = pass && (finalCpuState["a"].get<int>() == cpu.getRegisterValue("A"));
        pass = pass && (finalCpuState["x"].get<int>() == cpu.getRegisterValue("X"));
        pass = pass && (finalCpuState["y"].get<int>() == cpu.getRegisterValue("Y"));
        pass = pass && (finalCpuState["p"].get<int>() == cpu.getRegisterValue("P"));

        // check memory values
        for (auto itMem = finalMemState.begin(); itMem != finalMemState.end(); ++itMem)
        {
            int address = (*itMem)[0].get<int>();
            int value = (*itMem)[1].get<int>();

            finalMemory.push_back(address);
            finalMemory.push_back(value);

            pass = pass && (value == cpu.readTestMem(address));
        }

        pass ? passTest++ : failTest++;    

        if(!pass)
        {
            testResults << "FAILED TEST (Test Number: " << testNum << ")" << "\n";
            testResults << "-----------\n";

            testResults << format("{:<30}{}\n", "Initial CPU State", "Initial Actual CPU State");
            testResults << format("{:<30}{}\n", format("PC: {}", initialCpuState["pc"].get<int>()), initialTestPC);
            testResults << format("{:<30}{}\n", format("S: {}", initialCpuState["s"].get<int>()), initialTestS);
            testResults << format("{:<30}{}\n", format("A: {}", initialCpuState["a"].get<int>()), initialTestA);
            testResults << format("{:<30}{}\n", format("X: {}", initialCpuState["x"].get<int>()), initialTestX);
            testResults << format("{:<30}{}\n", format("Y: {}", initialCpuState["y"].get<int>()), initialTestY);
            testResults << format("{:<30}{}\n", format("P: {}", initialCpuState["p"].get<int>()), initialTestP);
            testResults << format("{:<30}{}\n", format("{:<10}{}", "Address", "Value"), format("{:<10}{}", "Address", "Value")); 
            for(int i = 0 ; i < initialMemory.size() ; i++)
            {
                int address = initialMemory.at(i);
                int initialTestValue = initialMemory.at(++i);
                int initialActualValue = initialMemory.at(++i);
                testResults << format("{:<30}{}\n", format("{:<10}: {:<10}", address, initialTestValue), format("{:<10}: {:<10}", address, initialActualValue));
            }
            testResults << "\n";

            testResults << format("{:<30}{}\n", "Expected Final CPU State", "Final Actual CPU State");
            testResults << format("{:<30}{}\n", format("PC: {}", finalCpuState["pc"].get<int>()), cpu.getRegisterValue("PC"));
            testResults << format("{:<30}{}\n", format("S: {}", finalCpuState["s"].get<int>()), cpu.getRegisterValue("S"));
            testResults << format("{:<30}{}\n", format("A: {}", finalCpuState["a"].get<int>()), cpu.getRegisterValue("A"));
            testResults << format("{:<30}{}\n", format("X: {}", finalCpuState["x"].get<int>()), cpu.getRegisterValue("X"));
            testResults << format("{:<30}{}\n", format("Y: {}", finalCpuState["y"].get<int>()), cpu.getRegisterValue("Y"));
            testResults << format("{:<30}{}\n", format("P: {}", finalCpuState["p"].get<int>()), cpu.getRegisterValue("P"));
            testResults << format("{:<30}{}\n", format("{:<10}{}", "Address", "Value"), format("{:<10}{}", "Address", "Value")); 
            for(int i = 0 ; i < finalMemory.size() ; i++)
            {
                int address = finalMemory.at(i);
                int value = finalMemory.at(++i);
                testResults << format("{:<30}{}\n", format("{:<10}: {:<10}", address, value), format("{:<10}: {:<10}", address, cpu.readTestMem(address)));
            }
            testResults << "\n\n";
        }    

        initialMemory.clear();
        finalMemory.clear();
        pass = true;
        testNum++;
    }

    testResults << "Pass: " << passTest << "\n";
    testResults << "Fail: " << failTest << "\n";
    testResults.close();
 
    return 0;
}