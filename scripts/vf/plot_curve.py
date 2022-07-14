exp_dict = {e:df.groupby('Expiry').groups[e].values for e in np.unique(df['Expiry'])}
n_strikes = df.shape[0]/expiries.size