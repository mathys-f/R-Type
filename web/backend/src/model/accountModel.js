/* Import modules */
import { DataTypes } from 'sequelize';
import sequelize from '../config/sequelize.js';

/* Define Account model */
const Account = sequelize.define('Account', {
    id: {
        type: DataTypes.INTEGER,
        autoIncrement: true,
        primaryKey: true
    },
    username: {
        type: DataTypes.STRING(100),
        allowNull: false,
        unique: true
    },
    email: {
        type: DataTypes.STRING(255),
        allowNull: false,
        unique: true
    },
    password_hash: {
        type: DataTypes.STRING(255)
    },
    total_kills: {
        type: DataTypes.INTEGER,
        defaultValue: 0
    },
    total_deaths: {
        type: DataTypes.INTEGER,
        defaultValue: 0
    },
    total_score: {
        type: DataTypes.INTEGER,
        defaultValue: 0
    },
    created_at: {
        type: DataTypes.DATE,
        defaultValue: DataTypes.NOW
    },
    last_login: {
        type: DataTypes.DATE,
        allowNull: true
    }
}, {
    tableName: 'accounts',
    timestamps: false
});

/* Export Account model */
export default Account;
