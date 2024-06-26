#if !defined(MMO_CORE_PLAYER_H)
#define MMO_CORE_PLAYER_H
#include <google/protobuf/message.h>
#include <zinx/inc/Packet.h>
#include <zinx/inc/ZConnection.h>
#include <zinx/inc/NonCopyable.h>
#include <memory>
#include <shared_mutex>

namespace mmo {

struct Position
{
    int X;    // length
    int Y;    // height
    int Z;    // width
    int V;    // view angle
};

class Grid;         // forward declaration
class Player;       // forward declaration
class WorldManager; // forward declaration
using PlayerPtr = std::shared_ptr<mmo::Player>;

/// Factory method
std::shared_ptr<Player> CreateNewPlayer(const zinx::ZinxConnectionPtr& conn, const Position& pos);

/// Represent a player
/// The associated TCP connection is held.
/// Should destroy the mmo::Player instance When corresponding Player offline,
/// so that disconnect the underlying connection
class Player : zinx::base::NonCopyable {
    friend std::shared_ptr<Player> CreateNewPlayer(const zinx::ZinxConnectionPtr& conn, const Position& pos);
    Player(int32_t pid, const zinx::ZinxConnectionPtr& conn, const Position& pos);
    
public:
    int32_t GetPid() const 
    { return pid_; }

    const Position& GetPosition() const
    {
        std::shared_lock<std::shared_mutex> guard(rwMutex_);
        return pos_; 
    }

    Position& GetPosition()
    { 
        std::shared_lock<std::shared_mutex> guard(rwMutex_);
        return pos_;
    }

    /// Sync the current player's PID to his client 
    void SyncPid();

    /// 将自身位置广播至所有玩家客户端(包括自己)
    /// 将周围玩家信息同步到当前玩家客户端
    void SyncWithSurrounding(const mmo::WorldManager& wm);

    /// Talk to all players
    void WorldChat(const std::string& content, const mmo::WorldManager& wm);

    /// Notify surrounding players that the current player is disappeared or offline
    void Disappear(const mmo::WorldManager& wm);
    
    /// Update the current client position and sync to surrounding players
    void UpdatePos(const Position& new_pos, const mmo::WorldManager& wm);

private:
    void SendPacket(const zinx::base::Packet& p)
    { conn_->Send(p.GetData()); }

    void HandleCrossedGrid(const Position& old_pos, const Position& new_pos, const mmo::WorldManager& wm);

private:
    const int32_t pid_;             // constant, doesn't need to sync
    zinx::ZinxConnectionPtr conn_;  // hold a connection handle
    
    Position pos_;
    mutable std::shared_mutex rwMutex_;   // for syncly access the field pos_
};

} // namespace mmo 

#endif // MMO_CORE_PLAYER_H
