/* Import modules */
import { DataTypes } from 'sequelize';
import sequelize from '../config/sequelize.js';

/* Define ActiveSession model */
const ActiveSession = sequelize.define('ActiveSession', {
    id: {
        type: DataTypes.INTEGER,
        autoIncrement: true,
        primaryKey: true
    },
    account_id: {
        type: DataTypes.INTEGER,
        allowNull: true,
        references: {
            model: 'accounts',
            key: 'id'
        }
    },
    player_name: {
        type: DataTypes.STRING(255),
        allowNull: false
    },
    lobby_id: {
        type: DataTypes.INTEGER,
        allowNull: true,
        references: {
            model: 'active_lobbies',
            key: 'id'
        }
    },
    session_token: {
        type: DataTypes.STRING(255),
        unique: true,
        allowNull: true
    },
    ip_address: {
        type: DataTypes.STRING(45),
        allowNull: true
    },
    connected_at: {
        type: DataTypes.DATE,
        defaultValue: DataTypes.NOW
    }
}, {
    tableName: 'active_sessions',
    timestamps: false,
    underscored: true
});

export default ActiveSession;
