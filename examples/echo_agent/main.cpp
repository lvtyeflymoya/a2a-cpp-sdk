#include <a2a/server/task_manager.hpp>
#include <a2a/server/memory_task_store.hpp>
#include <a2a/core/exception.hpp>
#include <iostream>
#include <string>
#include <exception>

using namespace a2a;

int main() {
    std::cout << "=== A2A C++ Echo Agent ===" << std::endl << std::endl;

    try
    {
        // === 1、初始化 ===
        // 利用内存存储方式创建task manager
        auto task_store = std::make_shared<MemoryTaskStore>();
        TaskManager task_manager(task_store);

        // === 2、设置消息处理器 ===
        // 这个回调函数的功能是接收MessageSendParams，包装成AgentMessage（A2A核心概念之一），然后返回A2AResponse
        task_manager.set_on_message_received([](const MessageSendParams& params) -> A2AResponse {
            std::cout << " Received message: " << params.message().get_text() << std::endl;

            // Echo back the message
            auto msg = AgentMessage::create()
                .with_role(MessageRole::Agent)
                .with_text("Echo: " + params.message().get_text());
            
            return A2AResponse(msg);
        });

        // === 3、设置agent身份信息 ===
        task_manager.set_on_agent_card_query([](const std::string& agent_url) -> AgentCard {
            AgentCapabilities caps;
            caps.streaming = true;
            caps.task_management = true;

            return AgentCard::create()
                .with_name("Echo Agent")
                .with_description("A simple echo agent that repeats your message")
                .with_url(agent_url)
                .with_version("1.0.0")
                .with_capabilities(caps)
                .with_input_mode("text")
                .with_output_mode("text");
        });

        // === 4、设置任务生命周期回调函数 ===
        task_manager.set_on_task_create([](const AgentTask& task) {
            std::cout << "Task created: " << task.id() << std::endl;
        });
        task_manager.set_on_task_updated([](const AgentTask& task) {
            std::cout << "Task updated: " << task.id()
                    << " - status: " << to_string(task.status().state()) << std::endl;
        });
        task_manager.set_on_task_cancelled([](const AgentTask& task) {
            std::cout << "Task Cancelled: " << task.id() << std::endl;
        });

        std::cout << "Echo Agent initialized successfully!" << std::endl;
        std::cout << "Ready to receive messages..." << std::endl << std::endl;

        // test message
        auto test_message = AgentMessage::create()
            .with_role(MessageRole::User)
            .with_text("Hello, Echo Agent!");

        auto test_params = MessageSendParams::create()
            .with_message(test_message);
        
        auto response = task_manager.send_message(test_params);
        
        if (response.is_message()) {
            std::cout << "Response: " << response.as_message().get_text() << std::endl;
        }
        std::cout << std::endl << "=== Agent demo completed ===" << std::endl;

        return 0;
    } catch (const A2AException& e) {
        std::cerr << "A2A Error: " << e.what() << std::endl;
        std::cerr << "Error code: " << e.error_code_value() << std::endl;
        return 1;
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
}