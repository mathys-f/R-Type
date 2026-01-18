/* Import modules */
import { DataTypes } from 'sequelize';
import sequelize from '../config/sequelize.js';

/* Define BannedPlayer model */
const BannedPlayer = sequelize.define('BannedPlayer', {
    id: {
        type: DataTypes.INTEGER,
        autoIncrement: true,
        primaryKey: true
    },
    player_name: {
        type: DataTypes.STRING(255),
        allowNull: true
    },
    account_id: {
        type: DataTypes.INTEGER,
        allowNull: true,
        references: {
            model: 'accounts',
            key: 'id'
        }
    },
    reason: {
        type: DataTypes.TEXT,
        allowNull: true
    },
    banned_at: {
        type: DataTypes.DATE,
        defaultValue: DataTypes.NOW
    },
    banned_by: {
        type: DataTypes.STRING(255),
        allowNull: true
    },
    expires_at: {
        type: DataTypes.DATE,
        allowNull: true
    }
}, {
    tableName: 'banned_players',
    timestamps: false,
    underscored: true
});

export default BannedPlayer;
