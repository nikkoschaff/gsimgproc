#!/home/gsdevel/gswebsite/env ruby
module Extest


tr = require_relative("Imgproc")
fname = "1.jpg"
fout = "~1.jpg"	

iproc = Imgproc.new

theStart = Time.now
resultValues = iproc.prepShowImage(fname,fout)

=begin
puts Time.now
count = 1
resultValues.each { |value|
count = 1
name = AnswerTranslator.translateName( value.pop )
puts "Test for: #{name}"
ansArr = AnswerTranslator.translateAllAnswers( value )
ansArr.each { |ans|
puts "#{count}: #{ans}"
count = count + 1
}
}
=end

puts "done"
count = 0
theEnd = Time.now

=begin
resultValues.each { |rvalue|
	puts fnames[count]
	puts rvalue.last.last
	count += 1
}
=end
puts theStart
puts theEnd

end
