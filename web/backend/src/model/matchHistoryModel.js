/* Import modules */
import { DataTypes } from 'sequelize';
import sequelize from '../config/sequelize.js';

/* Define MatchHistory model */
const MatchHistory = sequelize.define('MatchHistory', {
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
        allowNull: true
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
    match_date: {
        type: DataTypes.DATE,
        defaultValue: DataTypes.NOW
    }
}, {
    tableName: 'match_history',
    timestamps: false,
    underscored: true
});

export default MatchHistory;
