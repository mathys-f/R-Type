/* Import modules */
import { DataTypes } from 'sequelize';
import sequelize from '../config/sequelize.js';

/* Define PlayerScore model */
const PlayerScore = sequelize.define('PlayerScore', {
    id: {
        type: DataTypes.INTEGER,
        autoIncrement: true,
        primaryKey: true
    },
    session_id: {
        type: DataTypes.INTEGER,
        allowNull: false,
        references: {
            model: 'active_sessions',
            key: 'id'
        }
    },
    lobby_id: {
        type: DataTypes.INTEGER,
        allowNull: true,
        references: {
            model: 'active_lobbies',
            key: 'id'
        }
    },
    kills: {
        type: DataTypes.INTEGER,
        defaultValue: 0
    },
    deaths: {
        type: DataTypes.INTEGER,
        defaultValue: 0
    },
    score: {
        type: DataTypes.INTEGER,
        defaultValue: 0
    },
    updated_at: {
        type: DataTypes.DATE,
        defaultValue: DataTypes.NOW
    }
}, {
    tableName: 'player_scores',
    timestamps: false,
    underscored: true
});

export default PlayerScore;
