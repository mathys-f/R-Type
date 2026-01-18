/* Central model registry and associations */
import sequelize from '../config/sequelize.js';
import Account from './accountModel.js';
import AdminAccount from './adminAccountModel.js';
import MatchHistory from './matchHistoryModel.js';
import BannedPlayer from './bannedPlayerModel.js';
import ActiveSession from './activeSessionModel.js';
import PlayerScore from './playerScoreModel.js';
import ActiveLobby from './activeLobbyModel.js';
import EventLog from './eventLogModel.js';

/* Define associations */
Account.hasMany(MatchHistory, { foreignKey: 'account_id' });
MatchHistory.belongsTo(Account, { foreignKey: 'account_id' });

Account.hasMany(BannedPlayer, { foreignKey: 'account_id' });
BannedPlayer.belongsTo(Account, { foreignKey: 'account_id' });

AdminAccount.hasMany(EventLog, { foreignKey: 'admin_id' });
EventLog.belongsTo(AdminAccount, { foreignKey: 'admin_id' });

ActiveLobby.hasMany(ActiveSession, { foreignKey: 'lobby_id', onDelete: 'CASCADE' });
ActiveSession.belongsTo(ActiveLobby, { foreignKey: 'lobby_id' });

ActiveSession.hasOne(PlayerScore, { foreignKey: 'session_id', onDelete: 'CASCADE' });
PlayerScore.belongsTo(ActiveSession, { foreignKey: 'session_id' });

/* Export models and sequelize instance */
export {
  sequelize,
  Account,
  AdminAccount,
  MatchHistory,
  BannedPlayer,
  ActiveSession,
  PlayerScore,
  ActiveLobby,
  EventLog
};
