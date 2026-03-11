#pragma once

#include <a2a/server/task_store.hpp>
#include <hiredis/hiredis.h>
#include <memory>
#include <string>
#include <vector>
#include <mutex>
#include <stdexcept>

namespace a2a {

/**
 * @brief Redis base task store implementation for distributed deployment
 * 
 * This implementation uses redis to store tasks and history messages,
 * allowing multiple agents in different processes/machines to share the same storage
 */
class RedisTaskStore : public ITaskStore {
public:
    /**
     * @brief Construct with redis connection parameters
     * @param host redis host (default:localhost)
     * @param port redis post (default:6379)
     */
    explicit RedisTaskStore(const std::string& host = "127.0.0.1", int port = 6379);

    ~RedisTaskStore();

    // Disable copy, enable move
    RedisTaskStore(const RedisTaskStore&) = delete;
    RedisTaskStore& operator=(const RedisTaskStore&) = delete;
    RedisTaskStore(RedisTaskStore&&) noexcept = default;
    RedisTaskStore& operator=(RedisTaskStore&&) noexcept = default;

    // ITaskStore interface implementation
    std::optional<AgentTask> get_task(const std::string &task_id) override;
    void set_task(const AgentTask &task) override;
    bool task_exists(const std::string &task_id) override;
    bool delete_task(const std::string &task_id) override;
    void update_status(const std::string &task_id,
                       TaskState status,
                       const std::string &message = "") override;
    void add_artifact(const std::string &task_id,
                      const Artifact &artifact) override;
    void add_history_message(const std::string &task_id,
                             const AgentMessage &message) override;
    std::vector<AgentMessage> get_history(const std::string &context_id,
                                          int max_length = 0) override;

    
private:
    /**
     * @brief Get redis key for task
     */
    std::string task_key(const std::string& task_id) const {
        return "a2a:task:" + task_id;
    }

    /**
     * @brief Get redis key for history
     */
    std::string history_key(const std::string& context_id) const {
        return "a2a:history:" + context_id;
    }

    /**
     * @brief execute redis command and check errors
     */
    redisReply* execute_command(const char* format, ...);

    /**
     * @brief reconnect to redis if connection is lost
     */
    void ensure_connection();

    redisContext* context_;
    std::string host_;
    int port_;
    mutable std::mutex mutex_;
};
}   // namespace a2a