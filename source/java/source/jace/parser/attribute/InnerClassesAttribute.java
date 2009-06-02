package jace.parser.attribute;

import jace.parser.ClassAccessFlagSet;
import jace.parser.ConstantPool;
import jace.parser.constant.Constant;
import jace.parser.constant.UTF8Constant;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.util.Arrays;
import java.util.List;

/**
 * The InnerClasses attribute is a variable-length attribute in the attributes table of the 
 * ClassFile (§4.1) structure. 
 *
 * @author Toby Reyelts
 *
 */
public class InnerClassesAttribute implements Attribute {

  /* From the JVM specification.
   *
   * (u1 represents an unsigned byte)
   * (u2 represents an unsigned short)
   * (u4 represents an unsigned int)
   *
   * u2 attribute_name_index;
   * u4 attribute_length;
   * u2 number_of_classes;
   * {  u2 inner_class_info_index;	     
   *    u2 outer_class_info_index;	     
   *    u2 inner_name_index;	     
   *    u2 inner_class_access_flags;	     
   * } classes[number_of_classes];
   *
   */
  /**
   * Represents a single inner class.
   *
   */
  public class InnerClass {

    private int innerClassInfoIndex;
    private int outerClassInfoIndex;
    private int innerNameIndex;
    private int innerClassAccessFlags;

    InnerClass(DataInputStream input) throws IOException {
      innerClassInfoIndex = input.readUnsignedShort();
      outerClassInfoIndex = input.readUnsignedShort();
      innerNameIndex = input.readUnsignedShort();
      innerClassAccessFlags = input.readUnsignedShort();
    }

    public void write(DataOutputStream output) throws IOException {
      output.writeShort(innerClassInfoIndex);
      output.writeShort(outerClassInfoIndex);
      output.writeShort(innerNameIndex);
      output.writeShort(innerClassAccessFlags);
    }

    /**
     * Returns the full name of the inner class.
     *
     * The spec seems to indicate that this may not be set, but why?
     *
     * Originally, I thought it might be empty when the InnerClass
     * is in it's own class file, but that turns out not to be the case.
     * I have yet to come across a test case where this is empty.
     *
     */
    public String getInnerClassName() {
      return toString(mPool.getConstantAt(innerClassInfoIndex));
    }

    /**
     * Returns the name of the outer class.
     *
     * Originally I thought this would be empty when a class was nested but
     * not inner. (For example, a private class at top level). That turned 
     * out to be wrong - the public class is still represented as the outer class.
     *
     * The only test case that turns this value up as empty is for
     * an anonymous inner class (in it's own file, or a class that uses it). 
     * Since anonymous inner classes all have unreachable status, I'm not 
     * sure this is a problem to anyone.
     *
     */
    public String getOuterClassName() {
      return toString(mPool.getConstantAt(outerClassInfoIndex));
    }

    /**
     * Returns the simple name of the class. (Which is different from the compiler-generated
     * fully qualified name). It is possible that the inner class was anonymous, in which case
     * this method returns null.
     *
     */
    public String getName() {
      return toString(mPool.getConstantAt(innerNameIndex));
    }

    /**
     * Returns the access flags for this class. This will never be null, but
     * there may be no flags in the Set.
     *
     */
    public ClassAccessFlagSet getAccessFlags() {
      return new ClassAccessFlagSet(innerClassAccessFlags);
    }

    @Override
    public String toString() {
      return "Full Class Name: " + getInnerClassName() + "\n" +
        "Simple Class Name: " + getName() + "\n" +
        "Outer Class Name: " + getOuterClassName() + "\n" +
        "Access Flags: " + getAccessFlags() + "\n";
    }

    private String toString(Object obj) {
      return (obj == null) ? null : obj.toString();
    }
  }
  private ConstantPool mPool;
  private int mNameIndex;
  private int mLength;
  private InnerClass[] innerClasses;

  /**
   * Creates a new InnerClassesAttribute.
   *
   */
  public InnerClassesAttribute(InputStream stream, int nameIndex, ConstantPool pool) throws IOException {

    mPool = pool;
    mNameIndex = nameIndex;

    /* Read the name for this constant.
     * From the VM spec, we know it must be equal to "InnerClasses".
     */
    Constant c = mPool.getConstantAt(mNameIndex);

    if (c instanceof UTF8Constant) {

      String name = c.getValue().toString();

      if (!name.equals("InnerClasses")) {
        throw new ClassFormatError("While reading an InnerClassesAttribute, the name InnerClasses was expected, " +
          "but the name " + name + " was encountered.");
      }
    } else {
      throw new ClassFormatError("While reading an InnerClassesAttribute, a UTF8Constant was expected, " +
        "but a constant of type " + c.getClass().getName() + " was encountered.");
    }

    DataInputStream input = new DataInputStream(stream);

    /* Read the length of the attribute.
     * (An InnerClassesAttribute has variable length).
     */
    mLength = input.readInt();

    /* Read in all of the InnerClass information.
     */
    int numberOfClasses = input.readUnsignedShort();
    innerClasses = new InnerClass[numberOfClasses];

    for (int i = 0; i < numberOfClasses; ++i) {
      innerClasses[i] = new InnerClass(input);
    }
  }

  public void write(DataOutputStream output) throws IOException {
    output.writeShort(mNameIndex);
    output.writeInt(mLength);
    output.writeShort(innerClasses.length);
    for (int i = 0; i < innerClasses.length; ++i) {
      innerClasses[i].write(output);
    }
  }

  /**
   * Returns the name for this Attribute.
   *
   */
  public String getName() {
    return mPool.getConstantAt(mNameIndex).toString();
  }

  public String toString() {
    return getClass().getName();
  }

  /**
   * Returns the Constant that represents the value of this ConstantValueAttribute.
   *
   * Based on the type of the ClassField this ConstantValueAttribute belongs to,
   * the Constant may be one of the following:
   *
   *   long - LongConstant
   *   float - FloatConstant
   *   double - DoubleConstant
   *   int, short, char, byte, boolean - IntegerConstant
   *   String - StringConstant
   *
   * @return the Constant that represents the value of this ConstantValueAttribute
   */
  public List<InnerClass> getInnerClasses() {
    return Arrays.asList(innerClasses);
  }

  /**
   * Returns the length of this Attribute.
   *
   * @return the length of this Attribute
   */
  public int getLength() {
    return mLength;
  }
}

