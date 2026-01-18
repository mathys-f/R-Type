/* Import modules */
import Sequelize from 'sequelize';
import './dotenv.js'

/* Setup Sequelize instance */
const sequelize = new Sequelize(
    process.env.POSTGRES_URL,
    {
        dialect: 'postgres'
    }
);

/* Export Sequelize instance */
export default sequelize;
