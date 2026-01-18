/* Import modules */
import { DataTypes } from 'sequelize';
import sequelize from '../config/sequelize.js';

/* Define ActiveLobby model */
const ActiveLobby = sequelize.define('ActiveLobby', {
    id: {
        type: DataTypes.INTEGER,
        autoIncrement: true,
        primaryKey: true
    },
    name: {
        type: DataTypes.STRING(100),
        allowNull: false
    },
    port: {
        type: DataTypes.INTEGER,
        allowNull: true
    },
    max_players: {
        type: DataTypes.INTEGER,
        defaultValue: 4
    },
    player_count: {
        type: DataTypes.INTEGER,
        defaultValue: 0
    },
    created_at: {
        type: DataTypes.DATE,
        defaultValue: DataTypes.NOW
    }
}, {
    tableName: 'active_lobbies',
    timestamps: false,
    underscored: true
});

export default ActiveLobby;
