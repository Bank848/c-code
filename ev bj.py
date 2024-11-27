# กำหนดค่าความน่าจะเป็น (ค่าโดยประมาณ)
p_blackjack = 0.048  # ความน่าจะเป็นที่ผู้เล่นได้ Blackjack
p_player_bust = 0.28  # ความน่าจะเป็นที่ผู้เล่น Bust
p_dealer_bust = 0.17  # ความน่าจะเป็นที่เจ้ามือ Bust
p_player_win = 0.24  # ความน่าจะเป็นที่ผู้เล่นชนะ (ไม่รวม Blackjack)
p_player_lose = 0.23  # ความน่าจะเป็นที่ผู้เล่นแพ้
p_tie = 0.03  # ความน่าจะเป็นที่เสมอ

# กำหนดเงินเดิมพัน
bet = 1  # สมมติว่าเดิมพัน 1 หน่วย

# คำนวณผลลัพธ์ทางการเงินของแต่ละกรณี
ev_blackjack = bet * 2.5 * p_blackjack
ev_player_bust = -bet * p_player_bust
ev_dealer_bust = bet * 2 * p_dealer_bust
ev_player_win = bet * 2 * p_player_win
ev_player_lose = -bet * p_player_lose
ev_tie = 0  # เสมอไม่มีผลได้หรือเสีย

# คำนวณค่า EV รวม
expected_value = (
    ev_blackjack +
    ev_player_bust +
    ev_dealer_bust +
    ev_player_win +
    ev_player_lose +
    ev_tie
)

print(f"Expected Value (EV) ต่อการเล่นหนึ่งครั้ง: {expected_value:.2f}")
