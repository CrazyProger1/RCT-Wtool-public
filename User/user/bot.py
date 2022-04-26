class Bot:
    def __init__(self, bot_id, name, token, is_private, lot, parent_id):
        self.name = name
        self.id = bot_id
        self.token = token
        self.is_private = is_private
        self.lot = lot
        self.parent_id = parent_id

    def __repr__(self):
        return f"Bot<{self.name}>"
